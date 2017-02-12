// CoCo - Supernova templates and simulations package
// Copyright (C) 2016, 2017  Szymon Prajs
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Contact author: S.Prajs@soton.ac.uk

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
