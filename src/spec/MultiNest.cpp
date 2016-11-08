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


void LogLike(double *Cube, int &ndim, int &npars, double &lnew, void *context) {
    class Workspace *w = (struct Workspace *) context;

    // Apply the prior to the parameters
    for (size_t i = 0; i < npars; i++) {
        Cube[i] = flatPrior(Cube[i], 0, 100);
    }

    // Convert the C array of parameters to a C++ vector
    w->SNe_[w->SNID_].params_.resize(ndim);
    w->SNe_[w->SNID_].params_.assign(Cube, Cube + ndim);

	// spline control points
	vector<double> splineX(npars+2);
	vector<double> splineY(npars+2);

    // Set central control points
    for (size_t i = 0; i < npars; ++i) {
        splineX[i+1] = w->filters_->filters_[w->filters_->filterID_[w->SNe_[w->SNID_].lc_.filterList_[i]]].centralWavelength_;
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
	vector<double> sedCorrected(w->SNe_[w->SNID_].flux_.size(), 0);

	// mangle the spectra
	double sedMean = accumulate(w->SNe_[w->SNID_].flux_.begin(), w->SNe_[w->SNID_].flux_.end(), 0.0) / w->SNe_[w->SNID_].flux_.size();
	for (size_t i = 0; i < w->SNe_[w->SNID_].wav_.size(); ++i) {
		sedCorrected[i] = w->SNe_[w->SNID_].flux_[i] / sedMean;
        sedCorrected[i] *= gsl_spline_eval(spline, w->SNe_[w->SNID_].wav_[i], acc);
	}

    // Calculate likelihood
    lnew = 0;
    string filterName;
    sedCorrected = mult<double>(sedCorrected, min<double>(w->SNe_[w->SNID_].lcFlux_));
	for (size_t i = 0; i < w->SNe_[w->SNID_].lc_.filterList_.size(); ++i) {
		filterName = w->filters_->filters_[w->filters_->filterID_[w->SNe_[w->SNID_].lc_.filterList_[i]]].name_;
		lnew -= pow((w->filters_->flux(sedCorrected, filterName) - w->SNe_[w->SNID_].lcFlux_[i]) / w->SNe_[w->SNID_].lcFluxError_[i], 2);
	}
	lnew /= 2;

    w->SNe_[w->SNID_].fluxCorrected_ = sedCorrected;
}


MultiNest::MultiNest(shared_ptr<Workspace> w) {
    w_ = w;
}


void MultiNest::solve() {
    int seed = -1;			// random no. generator seed, if < 0 then take the seed from system clock
    int IS = 0;				// do Nested Importance Sampling?
	int mmodal = 0;			// do mode separation?
	int ceff = 0;			// run in constant efficiency mode?
	int nClsPar = 2;		// no. of parameters to do mode separation on
	int updInt = 10000;		// after how many iterations feedback & output files should be updated
	int maxModes = 1;		// expected max no. of modes (used only for memory allocation)
	int fb = 0;			    // need feedback on standard output?
	int resume = 0;			// resume from a previous job?
	int outfile = 1;		// write output files?
	int initMPI = 0;		// initialize MPI routines?, relevant only if compiling with MPI
    int maxiter = 0;		// max no. of iterations, a non-positive value means infinity.
    double Ztol = -1E90;	// all the modes with logZ < Ztol are ignored
    double tol = 0.1;		// tol, defines the stopping criteria
    double efr = 0.1;		// set the required efficiency
	double logZero = -1E90;	// points with loglike < logZero will be ignored by MultiNest

    void *context = (void*) w_.get();
}
