#include "SpecMangle.hpp"

#include <math.h>

#include <iostream>
#include <limits>

#include <gsl/gsl_bspline.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_errno.h>

#include "../vmath/loadtxt.hpp"
#include "../vmath/convert.hpp"
#include "../vmath/range.hpp"
#include "../vmath/stat.hpp"
#include "../vmath/algebra.hpp"


SpecMangle::SpecMangle() : Model() {
    noParams_ = 0;
    paramNames_ = {};

    priorRange_ = {{}};

    paramGuess_ = {};

    priorType_ = {};
}


//
double SpecMangle::function(double x) {
    int npars = 4;

    // spline control points
    std::vector<double> splineX(npars+2);
    std::vector<double> splineY(npars+2);

    // Set central control points
    for (size_t i = 0; i < npars; ++i) {
        splineX[i+1] = 0;
        splineY[i+1] = 0;
    }

    // Set first control point
    splineX[0] = 0; // vmath::min<double>(wav_);
    splineY[0] = splineX[0] * ((splineY[1] - splineY[2]) / (splineX[1] - splineX[2]));
    splineY[0] += ((splineX[1] * splineY[2] - splineY[1] * splineX[2]) / (splineX[1]-splineX[2]));

    // Set last control point
    splineX[npars+1] = 0; //vmath::max<double>(wav_);
    splineY[npars+1] = splineX[npars+1] * ((splineY[npars-1] - splineY[npars]) / (splineX[npars-1] - splineX[npars]));
    splineY[npars+1] += ((splineX[npars-1] * splineY[npars] - splineY[npars-1] * splineX[npars]) / (splineX[npars-1] - splineX[npars]));

    //initialise gsl spline
    gsl_interp_accel *acc = gsl_interp_accel_alloc();
    gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline, npars+2);
    gsl_spline_init (spline, splineX.data(), splineY.data(), npars+2);

    // mangle the spectra
    std::vector<double> returnSpline(4 /*flux_.size()*/, 0);
    for (size_t i = 0; i < 4 /*wav_.size()*/; ++i) {
        returnSpline[i] = gsl_spline_eval(spline, 4 /*wav_[i]*/, acc);
    }

    gsl_spline_free(spline);
    gsl_interp_accel_free(acc);
    return 0; // returnSpline;
}


std::vector<double> SpecMangle::residual() {
    return std::vector<double>(0);
}
