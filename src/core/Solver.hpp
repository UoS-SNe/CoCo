#ifndef COCO_CORE_SOLVER_HPP_
#define COCO_CORE_SOLVER_HPP_

#include <vector>
#include <string>
#include "../core/Model.hpp"


class Solver {
public:
    // Model parameters variables
    short noParams_;
    std::vector<double> initialParams_;
    std::vector<double> fitParams_;

    // Data to which the model is fitted
    std::vector<double> x_;
    std::vector<double> y_;
    std::vector<double> sigma_;

    // Model fit statistics
    std::vector<double> xRecon_;
    std::vector<double> bestFit_;
    std::vector<double> mean_;
    std::vector<double> meanSigma_;
    std::vector<double> median_;
    std::vector<double> medianSigma_;

    // Model class to be used in fitting
    std::shared_ptr<Model> model_;

    // Pure virtual functions that must be overriden by solver implementations
    virtual void fit() = 0;
    virtual void read() = 0;

    // Virtual functions with default behaviour
    virtual void stats();
    virtual void analyse();

    // Constructor
    Solver(std::shared_ptr<Model>);
};

#endif  // COCO_CORE_SOLVER_HPP_
