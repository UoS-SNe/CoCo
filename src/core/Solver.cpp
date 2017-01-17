#include "Solver.hpp"

#include <iostream>


// Constructor
Solver::Solver(std::shared_ptr<Model> model) : model_(model) {
    noParams_ = model_->noParams_;
    initialParams_ = model_->paramGuess_;

    // If no data is provided we replace the data with vectors of zeros
    // These are chosen to be number_of_parameters + 2 to prevent solver crashes
    // due to insufficient number of degrees of freedom
    x_ = std::vector<double>(noParams_ + 2, 0);
    y_ = std::vector<double>(noParams_ + 2, 0);
    sigma_ = std::vector<double>(noParams_ + 2, 0);
}


// If statistics cannot be produced by the solver class return vectors of zeros
// TODO - These vectors must have the size of the default x axis
// TEMP: *.size() = 1
void Solver::stats() {
    mean_ = std::vector<double>(1, 0);
    meanSigma_ = std::vector<double>(1, 0);
    median_ = std::vector<double>(1, 0);
    medianSigma_ = std::vector<double>(1, 0);
}


void Solver::analyse() {
    fit();
    read();
    stats();
}
