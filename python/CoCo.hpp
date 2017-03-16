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

#ifndef COCO_PYTHON_COCO_HPP_
#define COCO_PYTHON_COCO_HPP_

#include <vector>
#include <string>
#include <unordered_map>

#include "../src/core/Cosmology.hpp"
#include "../src/core/Filters.hpp"
#include "../src/core/SN.hpp"
#include "../src/core/Model.hpp"
#include "../src/core/Solver.hpp"


class CoCo {
public:
    // Constructor variables
    std::string filtersDir_;
    std::string reconRoot_;
    double H0_;
    double wm_;
    double wl_;

    // Simulation inputs
    std::string templateName_;
    double z_;
    double absMag_;
    double mjdPeak_;
    double Ebv_MW_;
    double Ebv_Host_;
    double R_v_;
    std::vector<double> mjdSim_;
    std::vector<std::string> filterSim_;

    // Templates
    std::unordered_map<std::string, SN> templateSNe_;

    // Shared data structures
    std::shared_ptr<Filters> filters_;
    std::shared_ptr<Cosmology> cosmology_;

    // Constructor
    CoCo();

    // routines
    void simulate();
};


#endif  // COCO_PYTHON_COCO_HPP_
