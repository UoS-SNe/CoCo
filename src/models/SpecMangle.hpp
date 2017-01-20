#ifndef COCO_MODELS_SPECMANGLE_HPP_
#define COCO_MODELS_SPECMANGLE_HPP_

#include <vector>
#include "../core/Model.hpp"
#include "../core/SN.hpp"


class SpecMangle : public Model {
public:
    // Variables and methods specific to the model
    std::vector<Obs> lcData_;
    SpecData specData_;

    // Constructor
    SpecMangle();

    // Override for the mathematical definition of the model
    double function(double);
    std::vector<double> residual();
};


#endif  // COCO_MODELS_SPECMANGLE_HPP_
