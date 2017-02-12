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

#include "MPFitter.hpp"

#include <vector>


MPFitter::MPFitter(std::shared_ptr<Model> model) : Solver(model) {
    // Fit parameters
    par = model->paramGuess_;
    parErr = std::vector<double>(par.size());

    // Memset MPFitter variable
    memset(&config, 0, sizeof(config));
    memset(&result, 0, sizeof(result));
    memset(&pars, 0, sizeof(par.data()));
    result.xerror = parErr.data();
    config.maxiter = 2000;
}


int MPFitter::resFunc(int m, int n, double *p, double *residual, double **dvec, void *vars) {
    Solver *solver = static_cast<Solver*>(vars);
    solver->model_->params_.assign(p, p + n);

    residual = (solver->model_->residual()).data();

    return 0;
}


void MPFitter::fit() {
    void *context = static_cast<void*>(this);

    status = mpfit(MPFitter::resFunc, xRecon_.size(), par.size(), par.data(), pars,
                   &config, context, &result);
}


void MPFitter::read() {
    model_->params_ = par;
}


#include <iostream>
void MPFitter::stats() {
    mean_ = std::vector<double>(xRecon_.size(), 0);
    meanSigma_ = std::vector<double>(xRecon_.size(), 0);
    median_ = std::vector<double>(xRecon_.size(), 0);
    medianSigma_ = std::vector<double>(xRecon_.size(), 0);

    bestFit_ = (*model_)(xRecon_);
    mean_ = (*model_)(xRecon_);
}
