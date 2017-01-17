#ifndef COCO_CORE_SOLVER_HPP_
#define COCO_CORE_SOLVER_HPP_

#include <vector>
#include <string>
#include "../core/Model.hpp"

class Solver {
public:
    // Working variables for parameter
    short noParams_;
    std::vector<double> fitParams_;
    std::vector<double> initialParams_;
    std::vector<double> lParams_;
    std::vector<double> uParams_;

    // Model class to be used in fitting
    std::shared_ptr<Model> model_;

    // Pure virtual functions that must be overwriten by solver implementations
    virtual void fit() = 0;
    virtual void read() = 0;

    // Virtual functions with default behaviour
    virtual void analyse();

    // Constructor
    Solver(std::shared_ptr<Model>);
};

#endif  // COCO_CORE_SOLVER_HPP_
