// CoCo - Supernova templates and simulations package
// Copyright (C) 2017  Szymon Prajs
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

#include "SpecMangle.hpp"

#include <math.h>

#include <iostream>
#include <limits>

#include <gsl/gsl_bspline.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_errno.h>

#include "../vmath/stat.hpp"
#include "../vmath/algebra.hpp"


SpecMangle::SpecMangle() : Model() {
    noParams_ = 0;
    paramNames_ = {};

    priorRange_ = {{}};

    paramGuess_ = {};

    priorType_ = {};
}


//
double SpecMangle::function(double x) {
    return 0;
}


void SpecMangle::setPriors() {
    noParams_ = lcData_.size();

    priorRange_ = std::vector< std::pair<double,double> >(noParams_,
        std::make_pair<double,double>(1e-3, 1e3));
    paramGuess_ = std::vector<double>(noParams_, 1.0);
    priorType_ = std::vector<std::string>(noParams_, "flat");
}


std::vector<double> SpecMangle::function(std::vector<double>& wav) {
    int npars = params_.size();

    // spline control points
    std::vector<double> splineX(npars+2);
    std::vector<double> splineY(npars+2);

    // Set central control points
    for (size_t i = 0; i < npars; ++i) {
        splineX[i+1] = lcData_[i].wav_;
        splineY[i+1] = params_[i];
    }

    // Set first control point
    splineX[0] = vmath::min<double>(wav) - 100;
    splineY[0] = splineX[0] * ((splineY[1] - splineY[2]) / (splineX[1] - splineX[2]));
    splineY[0] += ((splineX[1] * splineY[2] - splineY[1] * splineX[2]) /
                   (splineX[1]-splineX[2]));

    // Set last control point
    splineX[npars+1] =  vmath::max<double>(wav) + 100;
    splineY[npars+1] = splineX[npars+1] * ((splineY[npars-1] - splineY[npars]) /
                       (splineX[npars-1] - splineX[npars]));
    splineY[npars+1] += ((splineX[npars-1] * splineY[npars] - splineY[npars-1] *
                         splineX[npars]) / (splineX[npars-1] - splineX[npars]));

    //initialise gsl spline
    gsl_interp_accel *acc = gsl_interp_accel_alloc();
    gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline, npars+2);
    gsl_spline_init (spline, splineX.data(), splineY.data(), npars+2);

    // mangle the spectra
    std::vector<double> modelSpline(wav.size(), 0);
    for (size_t i = 0; i < wav.size(); ++i) {
        modelSpline[i] = gsl_spline_eval(spline, wav[i], acc);
    }

    gsl_spline_free(spline);
    gsl_interp_accel_free(acc);

    return vmath::mult<double>(specData_.flux_, modelSpline);
}


std::vector<double> SpecMangle::residual() {
    std::vector<double> mangledSpec = function(specData_.wav_);
    std::vector<double> res(lcData_.size(), 0);

    for (size_t i = 0; i < lcData_.size(); ++i) {
        res[i] = (lcData_[i].flux_ - filters_->flux(mangledSpec, lcData_[i].filter_)) / lcData_[i].fluxErr_;
    }

    return res;
}
