#ifndef COCO_LC_MULTINEST_HPP_
#define COCO_LC_MULTINEST_HPP_

#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include <stdio.h>
#include "Workspace.hpp"
#include "../core/multinest.h"
#include "../core/priors.hpp"
#include "../vmath/loadtxt.hpp"
#include "../vmath/convert.hpp"
#include "../vmath/range.hpp"
#include "../vmath/stat.hpp"

using namespace std;
using namespace vmath;


void dumper(int&, int&, int&, double**, double**, double**, double&, double&, double&, void*);
void LogLike(double*,int&,int&,double&,void*);


class MultiNest {
public:
    shared_ptr<Workspace> w_;
    string chainRoot;
    string filterRoot;

    vector<double> fitParams_;

    MultiNest(shared_ptr<Workspace>&);
    void setup();
    void solve();
    void read();
    void fit();
};

#endif
