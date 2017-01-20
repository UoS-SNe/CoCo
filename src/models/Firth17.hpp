#ifndef COCO_MODELS_FIRTH17_HPP_
#define COCO_MODELS_FIRTH17_HPP_

#include <vector>
#include "../core/Model.hpp"


class Firth17 : public Model {
public:
    // Variables and methods specific to the model

    // Constructor
    Firth17();

    // Override for the mathematical definition of the model
    double function(double);
    std::vector<double> residual();
};


#endif  // COCO_MODELS_FIRTH17_HPP_
