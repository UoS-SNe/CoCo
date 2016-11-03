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
	// for_spec_likelihood *FSL = (for_spec_likelihood *) context;
	// // fill the parameters from cube
	// double params[FSL->npar];
	// for(size_t i = 0; i < FSL->npar; i++) {
	// 	params[i] = flatPrior(Cube[i], 0, 100);
	// }
	// double param_l[FSL->npar];
    //
	// //todo fill with correct values from filter responsec!!!
	// // Set the x position for the spline
	// for (int m = 0; m < FSL->npar; m++) {
	// 	if (FSL->Spec_info[1][m] == 'g')
	// 		param_l[m]= 4735.00 ;
	// 	else if (FSL->Spec_info[1][m] == 'r')
	// 		param_l[m]= 6200;
	// 	else if (FSL->Spec_info[1][m] == 'i')
	// 		param_l[m]= 7480;
	// 	else if (FSL->Spec_info[1][m] == 'z')
	// 		param_l[m]= 8805;
	// 	else if (FSL->Spec_info[1][m] == 'B')
	// 		param_l[m]= 4365;
	// 	else if (FSL->Spec_info[1][m] == 'V')
	// 		param_l[m]= 5455;
	// 	else if (FSL->Spec_info[1][m] == 'R')
	// 		param_l[m]= 6485;
	// 	else if (FSL->Spec_info[1][m] == 'I')
	// 		param_l[m]= 8040;
	//  }
    //
	// // spline control points
	// vector<double> spline_x(FSL->npar+2);
	// vector<double> spline_y(FSL->npar+2);
    //
	// // Set 1st and last x spline control point
	// spline_x[0] = *min_element(FSL->wavelength.begin(),FSL->wavelength.end());
	// spline_x[FSL->npar+1] = *max_element(FSL->wavelength.begin(),FSL->wavelength.end());
	// // Set spline x and y mid control points
	// for (int q = 1; q <= FSL->npar; q++ )
	// 	spline_x[q] = param_l[q-1];
	// for (int w = 1; w <= FSL->npar; w++ )
	// 	spline_y[w] = params[w-1];
    //
	// //Set 1st and last y control points
	// spline_y[0] = spline_x[0] * ((spline_y[1] - spline_y[2]) / (spline_x[1] - spline_x[2]))
    // spline_y[0] += ((spline_x[1] * spline_y[2] - spline_y[1] * spline_x[2]) / (spline_x[1]-spline_x[2]));
	// spline_y[FSL->npar+1] = spline_x[FSL->npar+1] * ((spline_y[FSL->npar-1] - spline_y[FSL->npar]) / (spline_x[FSL->npar-1] - spline_x[FSL->npar]))
    // spline_y[FSL->npar+1] += ((spline_x[FSL->npar-1] * spline_y[FSL->npar] - spline_y[FSL->npar-1] * spline_x[FSL->npar]) / (spline_x[FSL->npar-1] - spline_x[FSL->npar]));
    //
	// //initialise gsl spline
	// gsl_interp_accel *acc = gsl_interp_accel_alloc();
	// gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline, FSL->npar+2);
	// gsl_spline_init (spline, spline_x.data(), spline_y.data(), FSL->npar+2);
	// vector<double> SED_corrected(FSL->SED.size(), 0);
    //
	// // mangle the spectra
	// double SED_max = *max_element(FSL->SED.begin(), FSL->SED.end());
	// double SED_min = *min_element(FSL->SED.begin(), FSL->SED.end());
	// double SED_mean = accumulate(FSL->SED.begin(), FSL->SED.end(), 0.0) / FSL->SED.size();
    //
    // double mangle;
	// for (size_t i = 0; i < FSL->wavelength.size(); ++i) {
	// 	mangle = gsl_spline_eval(spline, FSL->wavelength[i], acc);
	// 	SED_corrected[i] = FSL->SED[i] * mangle / SED_mean;
	// }
    //
    //
	// double sigma = 0.01;
	// string temp_name, temp_name_next;
	// lnew = 0;
    //
	// double LCphot_min = *min_element(FSL->LC.begin(), FSL->LC.end());
	// for ( int i = 0; i < FSL->wavelength.size(); i++ ) {
	// 	SED_corrected[i] *= LCphot_min;
	// }
    //
    //
	// for (size_t q = 0; q < FSL->Spec_info[1].size(); ++q) {
	// 	stringstream ss;
	// 	ss << FSL->Spec_info[1][q];
	// 	ss >> temp_name;
	// 	lnew -= pow((FSL->filter->flux(SED_corrected, "SDSS_"+temp_name) - (FSL->LC[q])) / (FSL->LC_err[q]), 2);
    //
	// }
	// lnew /= 2;
    //
    // FSL->SED_new = SED_corrected;
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
