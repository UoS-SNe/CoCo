#ifndef COCO_SOLVER_HPP_
#define COCO_SOLVER_HPP_

#include <vector>
#include <string>
#include "Workspace.hpp"

using namespace std;


class Solver {
public:
    shared_ptr<Workspace> w_;

    vector<double> fitParams_;
    vector<double> mean;
    vector<double> sigma;
    vector<double> median;
    vector<double> median_sigma;

    Solver(shared_ptr<Workspace> &w);
    virtual void fit() = 0;
};

#endif
