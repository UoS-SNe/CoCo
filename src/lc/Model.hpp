#ifndef COCO_MODEL_HPP_
#define COCO_MODEL_HPP_

#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <math.h>

using namespace std;

class Model {
public:
    short npar_;
    vector<double> params_;
    vector<short> priorType_;
    vector< vector<double> > priorRange_;

    Model();
    double func(double);
    double operator()(double);
    vector<double> operator()(vector<double>&);
};


#endif
