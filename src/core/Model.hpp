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

#ifndef COCO_CORE_MODEL_HPP_
#define COCO_CORE_MODEL_HPP_

#include <vector>
#include <string>
#include <utility>


class Model {
public:
    short noParams_;
    std::vector<double> params_;
    std::vector<std::string> priorType_;
    std::vector<std::string> paramNames_;
    std::vector<double> paramGuess_;
    std::vector< std::pair<double,double> > priorRange_;

    // Constructor
    Model();

    // Mathematical expression describing the model as a function of the x-axis
    // Must be overriden by child model classes
    virtual double function(double) = 0;
    virtual std::vector<double> function(std::vector<double>&) = 0;
    virtual std::vector<double> residual() = 0;

    // Functors providing model evaluations
    double operator()(double);
    std::vector<double> operator()(std::vector<double>&);
    void operator()(std::vector<double>&, std::vector<double>&);
};


#endif  // COCO_CORE_MODEL_HPP_
