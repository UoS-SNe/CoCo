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

#ifndef COCO_SOLVERS_MINUIT_HPP_
#define COCO_SOLVERS_MINUIT_HPP_

#include "mpfit.h"

#include <Minuit2/FunctionMinimum.h>
#include <Minuit2/MnUserParameterState.h>
#include <Minuit2/FCNBase.h>
#include <Minuit2/MnSimplex.h>
#include <Minuit2/MnMigrad.h>
#include <Minuit2/MnMinos.h>

#include "../core/Solver.hpp"
#include "../core/Model.hpp"

using namespace ROOT::Minuit2;


class MinuitResidual : public FCNBase {
public:
  MinuitResidual(std::shared_ptr<Model>&);
  ~MinuitResidual() {}

  virtual double Up() const {return error_definition_;}
  virtual double operator()(const std::vector<double>&) const;

private:
	std::shared_ptr<Model> model_;
	double error_definition_;
};


class Minuit : public Solver {
public:
    //Minuit globals
    std::vector<double> par;

    // Overrides for solver specific methods
    void fit();
    void read();
    void stats();

    // Constructor
    Minuit(std::shared_ptr<Model>);
};


#endif  // COCO_SOLVERS_MINUIT_HPP_
