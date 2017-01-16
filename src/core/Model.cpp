#include "Model.hpp"


Model::Model() {}


double Model::operator()(double x) {
    return function(x);
}


std::vector<double> Model::operator()(std::vector<double> &x) {
    std::vector<double> result(x.size());
    for (size_t i = 0; i < x.size(); ++i) {
        result[i] = function(x[i]);
    }

    return result;
}


void Model::operator()(std::vector<double> &x, std::vector<double> &result) {
    if (result.size() != x.size()) {
        result.resize(x.size());
    }

    for (size_t i = 0; i < x.size(); ++i) {
        result[i] = function(x[i]);
    }
}
