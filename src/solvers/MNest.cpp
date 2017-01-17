#include "MNest.hpp"

#include <iterator>
#include <string>

#include "multinest.h"
#include "../vmath/convert.hpp"
#include "../vmath/loadtxt.hpp"
#include "../vmath/stat.hpp"

#include "priors.hpp"


MNest::MNest(std::shared_ptr<Model> model) : Solver(model) {}

void MNest::fit() {

}


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
    // TODO - Calculate the best fit model and store in Solver class

    // Load and vmath::transpose post_equal_weights file used to Monte Carlo the light curve
    // PEW <=> Post Equal Weights
    string PewPath = rootPath + "post_equal_weights.dat";
    std::vector< std::vector<double> > Pew = vmath::loadtxt<double>(PewPath, noParams_ + 1);
    Pew = vmath::transpose<double>(Pew);

    // Initialise the light curve reconstruction vectors
    // w_->dataRecon_.y_ = vector<double>(w_->dataRecon_.x_.size(), 0);
    // w_->dataRecon_.sigma_ = vector<double>(w_->dataRecon_.x_.size(), 0);
    // w_->dataRecon_.median_ = vector<double>(w_->dataRecon_.x_.size(), 0);
    // w_->dataRecon_.medianSigma_ = vector<double>(w_->dataRecon_.x_.size(), 0);

    // For each PEW point calculate the model and append to the correct vector
    // vector< vector<double> > ModelCube(Pew.size());
    // for (size_t i = 0; i < Pew.size(); ++i) {
    //     w_->model_.params_.assign(Pew[i].begin(), Pew[i].end()-1);
    //     ModelCube[i] = w_->model_(w_->dataRecon_.x_);
    // }

    // For each simulated data point calculate the stats
    // ModelCube = vmath::transpose<double>(ModelCube);
    // for (size_t i = 0; i < ModelCube.size(); ++i) {
    //     w_->dataRecon_.y_[i] = mean<double>(ModelCube[i]);
    //     w_->dataRecon_.sigma_[i] = stdev<double>(ModelCube[i]);
    //     w_->dataRecon_.median_[i] = median<double>(ModelCube[i]);
    //     w_->dataRecon_.medianSigma_[i] = medianSigma<double>(ModelCube[i]);
    // }
}
