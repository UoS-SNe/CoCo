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

#ifndef COCO_MODELS_BAZIN09_HPP_
#define COCO_MODELS_BAZIN09_HPP_

#include <vector>
#include "../core/Model.hpp"


class Bazin09 : public Model {
public:
    // Data specific to the model
    std::vector<double> x_;
    std::vector<double> y_;
    std::vector<double> sigma_;

    // Constructor
    Bazin09();

    // Override for the mathematical definition of the model
    double function(double);
    std::vector<double> function(std::vector<double>&);
    std::vector<double> residual();
};


#endif  // COCO_MODELS_BAZIN09_HPP_
