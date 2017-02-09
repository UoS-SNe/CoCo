#include "MNest.hpp"

#include <iterator>
#include <string>
#include <limits>

#include "multinest.h"
#include "../vmath/convert.hpp"
#include "../vmath/loadtxt.hpp"
#include "../vmath/stat.hpp"

#include "priors.hpp"


MNest::MNest(std::shared_ptr<Model> model) : Solver(model) {
    livePoints_ = 100;
}


void MNest::dumper(int &nSamples, int &nlive, int &nPar, double **physLive, double **posterior, double **paramConstr, double &maxLogLike, double &logZ, double &logZerr, void *context) {
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


void MNest::logLike(double *Cube, int &ndim, int &npars, double &lnew, void *context) {
    Solver *solver = static_cast<Solver*>(context);

    // Apply the prior to the parameters
    for (size_t i = 0; i < npars; i++) {
        if (solver->model_->priorType_[i] == "flat") {
            Cube[i] = flatPrior(Cube[i], solver->model_->priorRange_[i].first, solver->model_->priorRange_[i].second);
        } else if (solver->model_->priorType_[i] == "log") {
            Cube[i] = logPrior(Cube[i], solver->model_->priorRange_[i].first, solver->model_->priorRange_[i].second);
        }
    }
    // Convert the C array of parameters to a C++ vector
    solver->model_->params_.assign(Cube, Cube + ndim);

    // log(Likelihood) function
    lnew = 0;
    std::vector<double> res = solver->model_->residual();
    for (size_t i = 0; i < res.size(); ++i) {
        if (res[i] == std::numeric_limits<double>::max()) {
            lnew = -std::numeric_limits<double>::max();
            break;
        } else {
            lnew -= pow(res[i], 2.0);
        }
    }
    lnew /= 2.0;
}


void MNest::fit() {
    int mmodal = 1;					// do mode separation?
    int ceff = 0;					// run in constant efficiency mode?
    double efr = 0.1;				// set the required efficiency
    double tol = 0.05;				// tol, defines the stopping criteria
    int ndims = noParams_;	        // dimensionality (no. of free parameters)
    int nPar = ndims;				// total no. of parameters including free & derived parameters
    int nClsPar = ndims;			// no. of parameters to do mode separation on
    int nlive = noParams_ * livePoints_;	// number of live points
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

    _rootPath = chainPath_ + "-";		            // root for output files
    void *context = static_cast<void*>(this);	// not required by MultiNest, any additional information user wants to pass

    // calling MultiNest
    nested::run(IS, mmodal, ceff, nlive, tol, efr, ndims, nPar, nClsPar,
                maxModes, updInt, Ztol, _rootPath.c_str(), seed, pWrap, fb, resume,
                outfile, initMPI, logZero, maxiter, MNest::logLike, MNest::dumper, context);
}


// MultiNest does not store fit results in memory so the results need to be
// read from a text file.
void MNest::read() {
    // Load summary file containing best fit parameters
    std::string summaryPath = _rootPath + "summary.txt";
    std::vector< std::vector<double> > summary = vmath::loadtxt<double>(summaryPath, noParams_ * 4 + 2);

    // Find the highest logLike e.g. Best fit
    std::vector<double> logLike = summary[noParams_ * 4 + 1];
    size_t indexBest = std::distance(logLike.begin(), std::max_element(logLike.begin()+1, logLike.end()));

    // Load the parameters for the best fit
    fitParams_ = std::vector<double>(noParams_, 0);
    for (size_t i = 0; i < noParams_; ++i) {
        fitParams_[i] = summary[2 * noParams_ + i][indexBest];
    }

    // Find the light curve for the best fit parameters
    model_->params_ = fitParams_;
    bestFit_ = (*model_)(xRecon_);
}


// MultiNest produces a set of posterior weighted points which can be used to
// trivially calculate the statistics of the model
void MNest::stats() {
    // Load and transpose post_equal_weights points vector
    // pew <=> Post Equal Weights
    string pewPath = _rootPath + "post_equal_weights.dat";
    std::vector< std::vector<double> > pew = vmath::loadtxt<double>(pewPath, noParams_ + 1);
    pew = vmath::transpose<double>(pew);

    // Initialise the light curve reconstruction vectors
    mean_ = std::vector<double>(xRecon_.size(), 0);
    meanSigma_ = std::vector<double>(xRecon_.size(), 0);
    median_ = std::vector<double>(xRecon_.size(), 0);
    medianSigma_ = std::vector<double>(xRecon_.size(), 0);

    // For each PEW point calculate the model and append to the correct vector
    vector< vector<double> > modelCube(pew.size());
    for (size_t i = 0; i < pew.size(); ++i) {
        model_->params_.assign(pew[i].begin(), pew[i].end()-1);
        modelCube[i] = (*model_)(xRecon_);
    }

    // For each simulated data point calculate the stats
    modelCube = vmath::transpose<double>(modelCube);
    for (size_t i = 0; i < modelCube.size(); ++i) {
        mean_[i] = vmath::mean<double>(modelCube[i]);
        meanSigma_[i] = vmath::stdev<double>(modelCube[i]);
        median_[i] = vmath::median<double>(modelCube[i]);
        medianSigma_[i] = vmath::medianSigma<double>(modelCube[i]);
    }
}
