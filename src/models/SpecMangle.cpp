#include "SpecMangle.hpp"

#include <math.h>

#include <iostream>
#include <limits>

#include <gsl/gsl_bspline.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_errno.h>

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
    return x;
}


std::vector<double> SpecMangle::residual() {
    int npars = params_.size();

    // spline control points
    std::vector<double> splineX(npars+2);
    std::vector<double> splineY(npars+2);

    // Set central control points
    for (size_t i = 0; i < npars; ++i) {
        splineX[i+1] = lcData_[i].wav_;
        splineY[i+1] = params_[i];
    }

    // Set first control point
    splineX[0] = vmath::min<double>(specData_.wav_) - 100;
    splineY[0] = splineX[0] * ((splineY[1] - splineY[2]) / (splineX[1] - splineX[2]));
    splineY[0] += ((splineX[1] * splineY[2] - splineY[1] * splineX[2]) / (splineX[1]-splineX[2]));

    // Set last control point
    splineX[npars+1] =  vmath::max<double>(specData_.wav_) + 100;
    splineY[npars+1] = splineX[npars+1] * ((splineY[npars-1] - splineY[npars]) / (splineX[npars-1] - splineX[npars]));
    splineY[npars+1] += ((splineX[npars-1] * splineY[npars] - splineY[npars-1] * splineX[npars]) / (splineX[npars-1] - splineX[npars]));

    //initialise gsl spline
    gsl_interp_accel *acc = gsl_interp_accel_alloc();
    gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline, npars+2);
    gsl_spline_init (spline, splineX.data(), splineY.data(), npars+2);

    // mangle the spectra
    std::vector<double> modelSpline(specData_.wav_.size(), 0);
    for (size_t i = 0; i < specData_.wav_.size(); ++i) {
        modelSpline[i] = gsl_spline_eval(spline, specData_.wav_[i], acc);
    }

    gsl_spline_free(spline);
    gsl_interp_accel_free(acc);

    return vmath::sub<double>(specData_.flux_, modelSpline);
}
