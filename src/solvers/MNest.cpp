#include "MNest.hpp"

#include <iterator>
#include <string>
#include <limits>

#include "multinest.h"
#include "../vmath/convert.hpp"
#include "../vmath/loadtxt.hpp"
#include "../vmath/stat.hpp"

#include "priors.hpp"


MNest::MNest(std::shared_ptr<Model> model) : Solver(model) {}


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
    class Solver *solver = (class Solver*) context;

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
    double modelFlux;
    // for (size_t i = 0; i < DATA.X_.size(); ++i) {
    //     modelFlux = model(DATA.X_[i]);
    //     if (modelFlux < 0.0) {
    //         lnew = -std::numeric_limits<double>::max();
    //         break;
    //     }
    //     lnew -= pow((DATA.Y_[i] - modelFlux) / DATA.sigma_[i], 2.0);
    // }
    lnew /= 2.0;
}


void MNest::fit() {}


// MultiNest does not store fit results in memory so the results need to be
// read from a text file.
// TODO - Finish once I decide what to do with xRange_
void MNest::read() {
    // Load summary file containing best fit parameters
    std::string summaryPath = rootPath + "summary.txt";
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
    // xRecon_ = vmath::range<double>(min(x_data), max(x_data), 1);
    // bestFit_ = model_(xRecon_);
}


// MultiNest produces a set of posterior weighted points which can be used to
// trivially calculate the statistics of the model
// TODO - Finish once I decide what to do with xRange_
void MNest::stats() {
    // Load and transpose post_equal_weights points vector
    // pew <=> Post Equal Weights
    string pewPath = rootPath + "post_equal_weights.dat";
    std::vector< std::vector<double> > pew = vmath::loadtxt<double>(pewPath, noParams_ + 1);
    pew = vmath::transpose<double>(pew);

    // Initialise the light curve reconstruction vectors
    mean_ = std::vector<double>(1, 0);
    meanSigma_ = std::vector<double>(1, 0);
    median_ = std::vector<double>(1, 0);
    medianSigma_ = std::vector<double>(1, 0);

    // For each PEW point calculate the model and append to the correct vector
    vector< vector<double> > modelCube(pew.size());
    for (size_t i = 0; i < pew.size(); ++i) {
        model_->params_.assign(pew[i].begin(), pew[i].end()-1);
        // modelCube[i] = w_->model_(xRecon_);
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
