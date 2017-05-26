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

#include "Minuit.hpp"
#include <cstring>
#include <vector>
#include <iostream>
#include <memory>


MinuitResidual::MinuitResidual(std::shared_ptr<Model> &model) {
    model_ = model;
    error_definition_ = 1.0;
}

double MinuitResidual::operator()(const std::vector<double>& par) const {
    double chi2 = 0;
    model_->params_ = par;
    std::vector<double> residual = model_->residual();

    for (int i = 0; i < residual.size(); ++i) {
        chi2 += pow(residual[i], 2.0);
    }

    return chi2;
}


Minuit::Minuit(std::shared_ptr<Model> model) : Solver(model) {
}

void Minuit::fit() {
    MinuitResidual likelihood(model_);

    MnUserParameters upar;

    for (int i = 0; i < model_->paramGuess_.size(); ++i) {
        upar.Add(model_->paramNames_[i], model_->paramGuess_[i], 1*model_->paramGuess_[i], model_->priorRange_[i].first, model_->priorRange_[i].second);
    }

    // MnSimplex simplex(likelihood, upar);
    // FunctionMinimum min_function = simplex();

    // MnMigrad migrad(likelihood, upar);
    // FunctionMinimum min_function = migrad();

    MnMigrad minos(likelihood, upar);
    FunctionMinimum min_function = minos();

    for(int i = 0; i < model_->params_.size(); ++i) {
    }
    par = min_function.UserState().Params();
}

void Minuit::read() {
    model_->params_ = par;
}

void Minuit::stats() {
    mean_ = std::vector<double>(xRecon_.size(), 0);
    meanSigma_ = std::vector<double>(xRecon_.size(), 0);
    median_ = std::vector<double>(xRecon_.size(), 0);
    medianSigma_ = std::vector<double>(xRecon_.size(), 0);

    bestFit_ = (*model_)(xRecon_);
    mean_ = (*model_)(xRecon_);
}
