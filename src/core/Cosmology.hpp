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

#ifndef COCO_CORE_COSMOLOGY_HPP_
#define COCO_CORE_COSMOLOGY_HPP_

#include <vector>


const double SI_C = 299792458;
const double CGS_C = 29979245800;
const double CGS_H = 6.62606957e-27;
const double CGS_K = 1.3806488e-16;
const double CGS_SIGMA = 5.67e-5;

class Cosmology {
public:
    // input
    double z_;
    double a_;

    // required variables
    const double H0_ = 70;
    const double WM_ = 0.3;
    const double WV_ = 0.7;

    // results
    double comDis_;
    double comVol_;
    double lumDis_;
    double lumDisCGS_;

    // Constructor
    Cosmology(double z = 0);

    // Functions
    void set(double);
    double E(double);
    double findComDis(double);
    double findLumDis(double);
    double findComVol(double);
    std::vector<double> findDZ(double);
    std::vector<double> findDV(std::vector<double>&);
};

#endif
