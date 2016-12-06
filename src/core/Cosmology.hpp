#ifndef COCO_CORE_COSMOLOGY_HPP_
#define COCO_CORE_COSMOLOGY_HPP_

#include <math.h>
#include <vector>
#include <memory>
#include <iostream>
#include "../vmath/integrate.hpp"

using namespace std;

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
    vector<double> findDZ(double);
    vector<double> findDV(vector<double>&);
};

#endif
