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
        if (w->model_.priorType_[i] == 0) {
            Cube[i] = flatPrior(Cube[i], w->model_.priorRange_[i][0], w->model_.priorRange_[i][1]);
        } else if (w->model_.priorType_[i] == 1) {
            Cube[i] = logPrior(Cube[i], w->model_.priorRange_[i][0], w->model_.priorRange_[i][1]);
        }
    }
    // Convert the C array of parameters to a C++ vector
    w->model_.params_.assign(Cube, Cube + ndim);


    // basic log(Likelihood)
    lnew = 0;
    double modelFlux;
    for (size_t i = 0; i < w->data_.x_.size(); ++i) {
        modelFlux = w->model_(w->data_.x_[i]);
        if (modelFlux < 0.0) {
            lnew = -std::numeric_limits<double>::max();
            break;
        }
        lnew -= pow((w->data_.y_[i] - modelFlux) / w->data_.sigma_[i], 2.0);
    }
    lnew /= 2.0;

    // Some (serious) corrections for special cases (Natasha's magic)
    int iMax = distance(w->data_.y_.begin(), max_element(w->data_.y_.begin(), w->data_.y_.end()));
    int iMin = distance(w->data_.y_.begin(), min_element(w->data_.y_.begin(), w->data_.y_.end()));
    double tMax = w->data_.x_[iMax];
    double tMin = w->data_.x_[iMin];
    double fluxMax = w->data_.y_[iMax];
    double fluxMin = w->data_.y_[iMin];
    double tLast = max<double>(w->data_.x_);

    // Check if the earliest data point is the highest flux
    double sBreak;
    double fluxDiff;
    if (tMax == w->SNe_[w->SNID_].mjdMinList_[w->FLTID_]) {
        if (w->model_(tMax - 7) > w->model_(tMax - 6)) {
            lnew -= pow(((w->model_(tMax - 6) - w->model_(tMax - 7)) / 0.1), 2.0);
        }

        sBreak = 0;
        for (double t = tMax-7.0; t < tMax; t += 0.5) {
            fluxDiff = (w->model_(t + 0.5) - w->model_(t));
            fluxDiff /= (w->model_(t + 1.0) - w->model_(t + 0.5));

            if (fluxDiff < 0.0) {
                sBreak++;
                t += 0.5;
            }

            if (sBreak > 1.5) {
                lnew = -100 * fabs(lnew);
                break;
            }
        }
    }

    // This seems to be looking at early times
    if (((tMax - tMin) > 13.9 && (fluxMin > 0.4)) || ((tMax - tMin) > 19.0)) {
    } else {
        if (w->model_(tMax - 30) > 0.02) {
            lnew -= pow(((w->model_(tMax - 30) - 0.02) / 0.001), 2.0);
        }
        if (w->model_(tMax - 60) > 0.002) {
            lnew -= pow(((w->model_(tMax - 60) - 0.002) / 0.001), 2.0);
        }
    }

    // And this is at crazy late time
    if (w->model_(tMax + 160) > 0.1) {
        lnew -= pow(((w->model_(tMax + 160) - 0.2) / 0.1), 2.0);
    }

    double fluxCoeff = 5.0;
    if (((tMax > tMin) && (fluxMax / fluxMin > 1.4)) || (fluxMin < 0.0)) {
        fluxCoeff = 0.0;
    }

    double fluxCorr = fluxMin;
    for (double t = tMin - fluxCoeff; t > tMin - 40.0; t -= 0.1) {
        if (w->model_(t) > fluxCorr) {
            lnew -= pow(((w->model_(t) - fluxCorr) / 0.1), 2.0);
        }
        fluxCorr = w->model_(t);
    }

    for (size_t i = 0; i < w->data_.x_.size()-1; ++i) {
        if ((w->data_.x_[i+1] - w->data_.x_[i]) < 1.1) {
            continue;
        }

        sBreak = 0;
        for (double t = w->data_.x_[i]; t < w->data_.x_[i+1]; t += 0.5) {
            fluxDiff = (w->model_(t + 0.5) - w->model_(t));
            fluxDiff /= (w->model_(t + 1.0) - w->model_(t + 0.5));

            if (fluxDiff < 0.0) {
                sBreak++;
                t += 0.5;
            }

            if (sBreak > 1.5) {
                lnew =-100 * fabs(lnew);
                break;
            }

            if (w->model_(t) > max(w->data_.y_[i], w->data_.y_[i+1]) * 3.0) {
                lnew -= pow(((w->model_(t) - max(w->data_.y_[i], w->data_.y_[i+1]) * 2.0) / 0.1), 2.0);
            }
        }
    }

    fluxCorr = 0.0;
    for (double t = tMin; t < tLast; t++) {
        if ((w->model_(t) - fluxCorr) > 50.0) {
            lnew -= pow((((w->model_(t) - fluxCorr) / 1.0 - 3.5) / 1.0), 2.0);
        }
        fluxCorr = w->model_(t);
    }

    fluxCorr = numeric_limits<double>::max();
    for (double t = tLast + 10; t < tLast + 100; t += 5) {
        if (w->model_(t) > fluxCorr) {
            lnew -= pow(((w->model_(t) - fluxCorr) / 0.1), 2.0);
        }
        fluxCorr = w->model_(t);
    }
}



MultiNest::MultiNest(shared_ptr<Workspace> &w) {
    w_ = w;
    chainRoot = "chains/" + w_->SNe_[w_->SNID_].name_ + "/";
    chainRoot += w_->SNe_[w_->SNID_].name_ + "_";
}


void MultiNest::solve() {
    int mmodal = 1;					// do mode separation?
    int ceff = 0;					// run in constant efficiency mode?
    double efr = 0.1;				// set the required efficiency
    double tol = 0.05;				// tol, defines the stopping criteria
    int ndims = w_->model_.npar_;	// dimensionality (no. of free parameters)
    int nPar = ndims;				// total no. of parameters including free & derived parameters
    int nClsPar = ndims;			// no. of parameters to do mode separation on
    int nlive = 1000;				// number of live points
    int updInt = 10000;				// after how many iterations feedback is required & the output files should be updated
                                    // note: posterior files are updated & dumper routine is called after every updInt*10 iterations
    double Ztol = -1e90;			// all the modes with logZ < Ztol are ignored
    int maxModes = 10;				// expected max no. of modes (used only for memory allocation)
    int pWrap[ndims];				// which parameters to have periodic boundary conditions?
    for (size_t i = 0; i < ndims; ++i) {
        pWrap[i] = 0;
    }
    int seed = -1;					// random no. generator seed, if < 0 then take the seed from system clock
    int fb = 1;					    // need feedback on standard output?
    int resume = 0;					// resume from a previous job?
    int outfile = 1;				// write output files?
    int initMPI = 0;				// initialize MPI routines?, relevant only if compiling with MPI
                                    // set it to F if you want your main program to handle MPI initialization
    double logZero = -1e90;			// points with loglike < logZero will be ignored by MultiNest
    int maxiter = 0;				// max no. of iterations, a non-positive value means infinity. MultiNest will terminate if either it
                                    // has done max no. of iterations or convergence criterion (defined through tol) has been satisfied
    int IS = 0;					    // do Nested Importance Sampling?

    filterRoot = chainRoot + w_->SNe_[w_->SNID_].filterList_[w_->FLTID_] + "-";		    // root for output files
    void *context = (void*) w_.get();	// not required by MultiNest, any additional information user wants to pass

    // calling MultiNest
    nested::run(IS, mmodal, ceff, nlive, tol, efr, ndims, nPar, nClsPar,
                maxModes, updInt, Ztol, filterRoot.c_str(), seed, pWrap, fb, resume,
                outfile, initMPI, logZero, maxiter, LogLike, dumper, context);
}


void MultiNest::read() {
    // Load summary file containing best fit parameters
    string summaryPath = filterRoot + "summary.txt";
    vector< vector<double> > summary = loadtxt<double>(summaryPath, (w_->model_.npar_) * 4 + 2);

    // Find the highest logLike -> Best fit
    vector<double> logLike = summary[w_->model_.npar_ * 4 + 1];
    int indexBest = distance(logLike.begin(), max_element(logLike.begin()+1, logLike.end()));

    // Load the parameters for the best fit
    fitParams_ = vector<double>(w_->model_.npar_, 0);
    for (int i = 0; i < w_->model_.npar_; ++i) {
        fitParams_[i] = summary[2 * w_->model_.npar_ + i][indexBest];
    }

    // Create the time axes for the light curve in range: tmin-15 --> tmax+20
    w_->dataRecon_.x_ = range<double>(-15, w_->SNe_[w_->SNID_].mjdMax_ - w_->SNe_[w_->SNID_].mjdMin_ + 20, 1);

    // Find the light curve for the best fit parameters
    w_->model_.params_ = fitParams_;
    w_->dataRecon_.bestFit_ = w_->model_(w_->dataRecon_.x_);

    // Load and transpose post_equal_weights file used to Monte Carlo the light curve
    string PEWPath = filterRoot + "post_equal_weights.dat";

    // PEW <=> Post Equal Weights
    vector< vector<double> > PEW = loadtxt<double>(PEWPath, (w_->model_.npar_) + 1);
    PEW = transpose<double>(PEW);

    // Initialise the light curve reconstruction vectors
    w_->dataRecon_.y_ = vector<double>(w_->dataRecon_.x_.size(), 0);
    w_->dataRecon_.sigma_ = vector<double>(w_->dataRecon_.x_.size(), 0);
    w_->dataRecon_.median_ = vector<double>(w_->dataRecon_.x_.size(), 0);
    w_->dataRecon_.medianSigma_ = vector<double>(w_->dataRecon_.x_.size(), 0);

    // For each PEW point calculate the model and append to the correct vector
    vector< vector<double> > ModelCube(PEW.size());
    for (size_t i = 0; i < PEW.size(); ++i) {
        w_->model_.params_.assign(PEW[i].begin(), PEW[i].end()-1);
        ModelCube[i] = w_->model_(w_->dataRecon_.x_);
    }

    // For each simulated data point calculate the stats
    ModelCube = transpose<double>(ModelCube);
    for (size_t i = 0; i < ModelCube.size(); ++i) {
        w_->dataRecon_.y_[i] = mean<double>(ModelCube[i]);
        w_->dataRecon_.sigma_[i] = stdev<double>(ModelCube[i]);
        w_->dataRecon_.median_[i] = median<double>(ModelCube[i]);
        w_->dataRecon_.medianSigma_[i] = medianSigma<double>(ModelCube[i]);
    }
}


void MultiNest::fit() {
    solve();
    read();
}
