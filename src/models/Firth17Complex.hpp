#ifndef COCO_MODELS_FIRTH17COMPLEX_HPP_
#define COCO_MODELS_FIRTH17COMPLEX_HPP_

#include <vector>
#include "../core/Model.hpp"


class Firth17Complex : public Model {
public:
    // Data specific to the model
    std::vector<double> x_;
    std::vector<double> y_;
    std::vector<double> sigma_;

    // Constructor
    Firth17Complex();

    // Override for the mathematical definition of the model
    double function(double);
    std::vector<double> function(std::vector<double>&);
    std::vector<double> residual();
};


#endif  // COCO_MODELS_FIRTH17COMPLEX_HPP_
