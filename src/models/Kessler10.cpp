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

#include "Kessler10.hpp"

#include <math.h>

#include <iostream>
#include <limits>


Kessler10::Kessler10() : Model() {
    noParams_ = 6;
    paramNames_ = {"A",
                   "a1",
                   "a2",
                   "Trise",
                   "Tfall",
                   "t0"};

    priorRange_ = {{1e-5, 1000.0}, // A
                   {1e-5, 100.0}, // a1
                   {1e-5, 100.0}, // a2
                   {0.2, 100.0},   // Trise
                   {0.0, 100.0},   // Tfall
                   {0.0, 100.0}};  // t0


    paramGuess_ = {1.0,   // A
                   0.1,    // a1
                   0.01,   // a2
                   5.0,   // Trise
                   20.0,  // Tfall
                   1.0};  // t0

    priorType_ = {"log",  // A
                  "log",  // a1
                  "log",  // a2
                  "flat", // Trise
                  "flat", // Tfall
                  "flat"};// t0
}


double Kessler10::function(double t) {
    double flux = params_[0] * (1.0 + params_[1]*(t-params_[5]) + params_[2]*(t-params_[5]));
    flux *= exp(-(t-params_[5])/params_[4]);
    flux /= (1.0 + exp(-(t-params_[5])/params_[3]));

    return flux;
}


std::vector<double> Kessler10::function(std::vector<double>& t) {
    std::vector<double> res(t.size(), 0);
    for (size_t i = 0; i < t.size(); ++i) {
        res[i] = function(t[i]);
    }

    return res;
}


std::vector<double> Kessler10::residual() {
    std::vector<double> res(x_.size(), 0);

    double flux;
    for (size_t i = 0; i < x_.size(); ++i) {
        flux = function(x_[i]);
        if (flux < 0) {
            res[i] = std::numeric_limits<double>::max();
        } else {
            res[i] = (y_[i] - flux) / sigma_[i];
        }
    }

    return res;
}
