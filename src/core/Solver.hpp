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

#ifndef COCO_CORE_SOLVER_HPP_
#define COCO_CORE_SOLVER_HPP_

#include <vector>
#include <string>
#include "../core/Model.hpp"


class Solver {
public:
    // Model parameters variables
    short noParams_;
    std::vector<double> initialParams_;
    std::vector<double> fitParams_;
    std::string chainPath_;

    // Model fit statistics
    std::vector<double> xRecon_;
    std::vector<double> bestFit_;
    std::vector<double> mean_;
    std::vector<double> meanSigma_;
    std::vector<double> median_;
    std::vector<double> medianSigma_;

    // Model class to be used in fitting
    std::shared_ptr<Model> model_;

    // Pure virtual functions that must be overriden by solver implementations
    virtual void fit() = 0;
    virtual void read() = 0;

    // Virtual functions with default behaviour
    virtual void stats();
    virtual void analyse();

    // Constructor
    Solver(std::shared_ptr<Model>);
};

#endif  // COCO_CORE_SOLVER_HPP_
