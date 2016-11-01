#ifndef COCO_SOLVER_MULTINEST_HPP_
#define COCO_SOLVER_MULTINEST_HPP_

#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include <stdio.h>
#include "multinest.h"
#include "Solver.hpp"
#include "Workspace.hpp"
#include "priors.hpp"
#include "vmath/loadtxt.hpp"
#include "vmath/convert.hpp"
#include "vmath/range.hpp"

using namespace std;


void dumper(int&, int&, int&, double**, double**, double**, double&, double&, double&, void*);
void LogLike(double*,int&,int&,double&,void*);


class MultiNest : public Solver {
public:
    shared_ptr<Workspace> w_;
    string chainRoot;
    string filterRoot;

    MultiNest(shared_ptr<Workspace>&);
    void setup();
    void solve();
    void readSummary();
    void reconstruct();
    void fit();
};

#endif
