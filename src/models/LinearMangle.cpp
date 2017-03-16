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

#include "LinearMangle.hpp"

#include <math.h>

#include <iostream>
#include <limits>

#include <gsl/gsl_bspline.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_errno.h>

#include "../vmath/stat.hpp"
#include "../vmath/algebra.hpp"


LinearMangle::LinearMangle() : Model() {
    noParams_ = 2;
    paramNames_ = {"m", "a"};

    priorRange_ = {{-100, 100}, {-100, 100}};

    paramGuess_ = {1, 1};

    priorType_ = {"flat", "flat"};
}


//
double LinearMangle::function(double x) {
    return x * params_[0] * 1e-4 + params_[1] * 1e-4;
}


std::vector<double> LinearMangle::function(std::vector<double>& wav) {
    std::vector<double> res(wav.size());
    for (int i = 0; i < wav.size(); ++i) {
        res[i] = function(wav[i]);
    }

    return vmath::mult(specData_.flux_, res);
}


std::vector<double> LinearMangle::residual() {
    std::vector<double> mangling = function(specData_.wav_);
    std::vector<double> mangledSpec = vmath::mult(specData_.flux_, mangling);
    std::vector<double> res(lcData_.size(), 0);

    for (size_t i = 0; i < lcData_.size(); ++i) {
        res[i] = (lcData_[i].flux_ - filters_->flux(mangledSpec, lcData_[i].filter_)) / lcData_[i].fluxErr_;
    }

    return res;
}
