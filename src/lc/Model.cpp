#include "Model.hpp"

using namespace std;


Model::Model() {
    // Number of model paramters
    npar_ = 8;

    // priors ranges on coefficients
    priorRange_ = {{{1e-5, 1000.0}, // A
                    {1e-5, 100.0},  // B
                    {0.0, 100.0},   // t1
                    {0.2, 100.0},   // Trise
                    {0.0, 100.0},   // Tfall
                    {0.0, 100.0},   // t0
                    {0.0, 100.0},   // t2
                    {0.2, 100.0}}}; // Textra

    // prior type -- 0 = Flat, 1 = Log
    priorType_ = {1, 1, 0, 0, 0, 0, 0, 1};
}

double Model::func(double t) {
    double flux = params_[0] * (1.0 + params_[1] * pow(t - params_[2], 2.0));
    flux *= exp(-(t-params_[5])/params_[4]);
    flux /= (1.0 + exp(-(t - params_[5]) / params_[3]));
    flux /= (1.0 + exp(-(t - params_[6]) / params_[7]));

    return flux;
}

double Model::operator()(double t) {
    return func(t);
}


vector<double> Model::operator()(vector<double> &t) {
    vector<double> result(t.size());
    for (size_t i = 0; i < t.size(); ++i) {
        result[i] = func(t[i]);
    }

    return result;
}
