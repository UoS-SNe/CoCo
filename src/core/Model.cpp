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

#include "Model.hpp"


Model::Model() {}


double Model::operator()(double x) {
    return function(x);
}


std::vector<double> Model::operator()(std::vector<double> &x) {
    return function(x);
}


void Model::operator()(std::vector<double> &x, std::vector<double> &result) {
    if (result.size() != x.size()) {
        result.resize(x.size());
    }

    for (size_t i = 0; i < x.size(); ++i) {
        result[i] = function(x[i]);
    }
}
