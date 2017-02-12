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

#ifndef COCO_SOLVERS_MNEST_HPP_
#define COCO_SOLVERS_MNEST_HPP_

#include "multinest.h"

#include "../core/Solver.hpp"
#include "../core/Model.hpp"


class MNest : public Solver {
public:
    // MultiNest specific temporary data containers
    std::string _rootPath;
    int livePoints_;

    // static functions used by MultiNest
    static void dumper(int&,int&,int&,double**,double**,double**,double&,
                       double&,double&,void*);
    static void logLike(double*,int&,int&,double&,void*);

    // Overrides for solver specific methods
    void fit();
    void read();
    void stats();

    // Constructor
    MNest(std::shared_ptr<Model>);
};


#endif  // COCO_SOLVERS_MNEST_HPP_
