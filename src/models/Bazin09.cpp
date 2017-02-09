#include "Bazin09.hpp"

#include <math.h>

#include <iostream>
#include <limits>


Bazin09::Bazin09() : Model() {
    noParams_ = 4;
    paramNames_ = {"A",
                   "Trise",
                   "Tfall",
                   "t0"};

    priorRange_ = {{1e-5, 1000.0}, // A
                   {0.2, 100.0},   // Trise
                   {0.0, 100.0},   // Tfall
                   {0.0, 100.0}};  // t0


    paramGuess_ = {1.0,   // A
                   5.0,   // Trise
                   20.0,  // Tfall
                   1.0};  // t0

    priorType_ = {"log",  // A
                  "flat", // Trise
                  "flat", // Tfall
                  "flat"};// t0
}


double Bazin09::function(double t) {
    double flux = params_[0] * exp(-(t-params_[4])/params_[3]);
    flux /= (1.0 + exp(-(t-params_[4])/params_[2]));
    flux += params_[1];

    return flux;
}


std::vector<double> Bazin09::function(std::vector<double>& t) {
    std::vector<double> res(t.size(), 0);
    for (size_t i = 0; i < t.size(); ++i) {
        res[i] = function(t[i]);
    }

    return res;
}


std::vector<double> Bazin09::residual() {
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
