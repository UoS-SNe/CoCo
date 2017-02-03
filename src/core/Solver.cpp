#include "Solver.hpp"

#include <iostream>


// Constructor
Solver::Solver(std::shared_ptr<Model> model) : model_(model) {
    noParams_ = model_->noParams_;
    initialParams_ = model_->paramGuess_;
}


// If statistics cannot be produced by the solver class return vectors of zeros
// TODO - These vectors must have the size of the default x axis
// TEMP: *.size() = 1
void Solver::stats() {
    mean_ = std::vector<double>(xRecon_.size(), 0);
    meanSigma_ = std::vector<double>(xRecon_.size(), 0);
    median_ = std::vector<double>(xRecon_.size(), 0);
    medianSigma_ = std::vector<double>(xRecon_.size(), 0);
}


void Solver::analyse() {
    fit();
    read();
    stats();
}
