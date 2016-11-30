#include "MultiNest.hpp"


void dumper(int &nSamples, int &nlive, int &nPar, double **physLive, double **posterior, double **paramConstr, double &maxLogLike, double &logZ, double &logZerr, void *context) {
    // the posterior distribution
    // postdist will have nPar parameters in the first nPar columns & loglike value & the posterior probability in the last two columns
    double postdist[nSamples][nPar + 1];
    for (size_t i = 0; i < nPar + 1; ++i) {
        for (size_t j = 0; j < nSamples; ++j) {
            postdist[j][i] = posterior[0][i * nSamples + j];
        }
    }

    // last set of live points
    // pLivePts will have nPar parameters in the first nPar columns & loglike value in the last column
    double pLivePts[nlive][nPar + 1];
    for (size_t i = 0; i < nPar + 1; ++i) {
        for (size_t j = 0; j < nlive; ++j) {
            pLivePts[j][i] = physLive[0][i * nlive + j];
        }
    }
}


vector<double> splineModel(WorkspaceSpec *w){
    int npars = w->SNe_[w->SNID_].params_.size();

    // spline control points
    vector<double> splineX(npars+2);
    vector<double> splineY(npars+2);

    // Set central control points
    for (size_t i = 0; i < npars; ++i) {
        splineX[i+1] = w->SNe_[w->SNID_].lcCentralWavelength_[i];
        splineY[i+1] = w->SNe_[w->SNID_].params_[i];
    }

    // Set first control point
    splineX[0] = min<double>(w->SNe_[w->SNID_].wav_);
    splineY[0] = splineX[0] * ((splineY[1] - splineY[2]) / (splineX[1] - splineX[2]));
    splineY[0] += ((splineX[1] * splineY[2] - splineY[1] * splineX[2]) / (splineX[1]-splineX[2]));

    // Set last control point
    splineX[npars+1] = max<double>(w->SNe_[w->SNID_].wav_);
    splineY[npars+1] = splineX[npars+1] * ((splineY[npars-1] - splineY[npars]) / (splineX[npars-1] - splineX[npars]));
    splineY[npars+1] += ((splineX[npars-1] * splineY[npars] - splineY[npars-1] * splineX[npars]) / (splineX[npars-1] - splineX[npars]));

    //initialise gsl spline
    gsl_interp_accel *acc = gsl_interp_accel_alloc();
    gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline, npars+2);
    gsl_spline_init (spline, splineX.data(), splineY.data(), npars+2);
    vector<double> returnSpline(w->SNe_[w->SNID_].flux_.size(), 0);

    // mangle the spectra
    for (size_t i = 0; i < w->SNe_[w->SNID_].wav_.size(); ++i) {
        returnSpline[i] = gsl_spline_eval(spline, w->SNe_[w->SNID_].wav_[i], acc);
    }

    return returnSpline;
}


void LogLike(double *Cube, int &ndim, int &npars, double &lnew, void *context) {
    class WorkspaceSpec *w = (struct WorkspaceSpec *) context;

    // Apply the prior to the parameters
    for (size_t i = 0; i < npars; i++) {
        Cube[i] = flatPrior(Cube[i], -100, 100);
    }

    // Convert the C array of parameters to a C++ vector
    w->SNe_[w->SNID_].params_.resize(ndim);
    w->SNe_[w->SNID_].params_.assign(Cube, Cube + ndim);

    // Calculate the corrected spectrum
    vector<double> sedCorrected = mult<double>(w->SNe_[w->SNID_].flux_, splineModel(w));

    // Calculate likelihood
    lnew = 0;
    string filterName;
	for (size_t i = 0; i < w->SNe_[w->SNID_].lc_.filterList_.size(); ++i) {
		filterName = w->SNe_[w->SNID_].lcFilters_[i];
		lnew -= pow((w->filters_->flux(sedCorrected, filterName) - w->SNe_[w->SNID_].lcFlux_[i]) / w->SNe_[w->SNID_].lcFluxError_[i], 2);
	}
	lnew /= 2;
}


MultiNest::MultiNest(shared_ptr<WorkspaceSpec> w) {
    w_ = w;
    chainRoot = "chains/" + w_->SNe_[w_->SNID_].SNName_ + "/";
    chainRoot += w_->SNe_[w_->SNID_].SNName_ + "_";
}


void MultiNest::solve() {
    int seed = -1;			// random no. generator seed, if < 0 then take the seed from system clock
    int IS = 0;				// do Nested Importance Sampling?
	int mmodal = 1;			// do mode separation?
	int ceff = 0;			// run in constant efficiency mode?
    int nlive = 100;		// number of live points
	int updInt = 1000;		// after how many iterations feedback & output files should be updated
	int maxModes = 10;		// expected max no. of modes (used only for memory allocation)
	int fb = 0;			    // need feedback on standard output?
    int resume = 0;			// resume from a previous job?
	int outfile = 1;		// write output files?
	int initMPI = 0;		// initialize MPI routines?, relevant only if compiling with MPI
    int maxiter = 0;		// max no. of iterations, a non-positive value means infinity.
    double Ztol = -1E90;	// all the modes with logZ < Ztol are ignored
    double tol = 0.1;		// tol, defines the stopping criteria
    double efr = 0.1;		// set the required efficiency
	double logZero = -1E90;	// points with loglike < logZero will be ignored by MultiNest

    int nPar = w_->SNe_[w_->SNID_].lcFilters_.size();
    int ndims = nPar;
    int nClsPar = nPar;		// no. of parameters to do mode separation on
    int pWrap[ndims];		// which parameters to have periodic boundary conditions?
    for (size_t i = 0; i < ndims; ++i) {
        pWrap[i] = 0;
    }

    specRoot = chainRoot + to_string(int(w_->SNe_[w_->SNID_].mjd_)) + "-";  // root for output files
    void *context = (void*) w_.get();

    // calling MultiNest
    nested::run(IS, mmodal, ceff, nlive, tol, efr, ndims, nPar, nClsPar,
                maxModes, updInt, Ztol, specRoot.c_str(), seed, pWrap, fb, resume,
                outfile, initMPI, logZero, maxiter, LogLike, dumper, context);
}


void MultiNest::read() {
    int npars = w_->SNe_[w_->SNID_].lcFilters_.size();

    // Load summary file containing best fit parameters
    string summaryPath = specRoot + "summary.txt";
    vector< vector<double> > summary = loadtxt<double>(summaryPath, (npars) * 4 + 2);

    // Find the highest logLike -> Best fit
    vector<double> logLike = summary[w_->SNe_[w_->SNID_].lcFilters_.size() * 4 + 1];
    int indexBest = distance(logLike.begin(), max_element(logLike.begin()+1, logLike.end()));

    // Load the parameters for the best fit
    w_->SNe_[w_->SNID_].params_ = vector<double>(npars, 0);
    for (size_t i = 0; i < npars; ++i) {
        w_->SNe_[w_->SNID_].params_[i] = summary[2 * npars + i][indexBest];
    }

    // Reconstrunct the best fit mangled spectrum
    vector<double> spline = splineModel(w_.get());
    vector<double> sedCorrected = mult<double>(w_->SNe_[w_->SNID_].flux_, spline);
    sedCorrected = mult<double>(sedCorrected, w_->SNe_[w_->SNID_].normFlux_);

    // Save the spectrum into a reconstruction directory
    ofstream reconSpecFile;
    reconSpecFile.open("recon/" + w_->SNe_[w_->SNID_].SNName_ + "_" + to_string(int(w_->SNe_[w_->SNID_].mjd_)) + ".spec");

    ofstream reconSplineFile;
    reconSplineFile.open("recon/" + w_->SNe_[w_->SNID_].SNName_ + "_" + to_string(int(w_->SNe_[w_->SNID_].mjd_)) + ".spline");

    for (size_t i = 0; i < sedCorrected.size(); ++i) {
        reconSpecFile << w_->SNe_[w_->SNID_].wav_[i] / (1.0 + w_->SNe_[w_->SNID_].z_) << " " << sedCorrected[i] << '\n';
        reconSplineFile << w_->SNe_[w_->SNID_].wav_[i] / (1.0 + w_->SNe_[w_->SNID_].z_) << " " << spline[i] << '\n';
    }
    reconSpecFile.close();
    reconSplineFile.close();
}


void MultiNest::fit() {
    solve();
    read();
}
