#ifndef COCO_SPEC_MULTINEST_HPP_
#define COCO_SPEC_MULTINEST_HPP_

#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include <stdio.h>
#include "../core/multinest.h"
#include "../core/priors.hpp"
#include "../vmath/loadtxt.hpp"
#include "../vmath/convert.hpp"
#include "../vmath/range.hpp"
#include "../vmath/stat.hpp"
#include "Workspace.hpp"


void dumper(int&, int&, int&, double**, double**, double**, double&, double&, double&, void*);
void LogLike(double*,int&,int&,double&,void*);

class MultiNest {
public:
    shared_ptr<Workspace> w_;

    MultiNest(shared_ptr<Workspace>);
    void solve();
};

#endif
