#ifndef COCO_CORE_MODEL_HPP_
#define COCO_CORE_MODEL_HPP_

#include <vector>
#include <string>
#include <utility>


class Model {
public:
    short noParams_;
    std::vector<double> params_;
    std::vector<std::string> priorType_;
    std::vector<std::string> paramNames_;
    std::vector<double> paramGuess_;
    std::vector< std::pair<double,double> > priorRange_;

    // Constructor
    Model();

    // Mathematical expression describing the model as a function of the x-axis
    // Must be overriden by child model classes
    virtual double function(double) = 0;
    virtual std::vector<double> residual() = 0;

    // Functors providing model evaluations
    double operator()(double);
    std::vector<double> operator()(std::vector<double>&);
    void operator()(std::vector<double>&, std::vector<double>&);
};


#endif  // COCO_CORE_MODEL_HPP_
