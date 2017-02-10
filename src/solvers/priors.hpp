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

#ifndef COCO_SOLVERS_PRIORS_HPP_
#define COCO_SOLVERS_PRIORS_HPP_


double skillingScalePrior(double);
double logPrior(double,double,double);
double modJeffPrior(double,double,double);
double flatPrior(double,double,double);
double powerPrior(double,double,double,double);
double gaussianPrior(double,double,double);
double sinPrior(double,double,double);
double cauchyPrior(double,double,double);
double logNormalPrior(double,double,double);
double dierfc(double);


#endif  // COCO_SOLVERS_PRIORS_HPP_
