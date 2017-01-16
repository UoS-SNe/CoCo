#ifndef COCO_CORE_SOLVER_HPP_
#define COCO_CORE_SOLVER_HPP_

#include <vector>
#include <string>

class Solver {
public:
    short noParams_;
    std::vector<double> fitParams_;
    std::vector<double> initialParams_;
    std::vector<double> lParams_;
    std::vector<double> uParams_;

    // Pure virtual functions that must be overwriten by solver implementations
    virtual void fit() = 0;
    virtual void read() = 0;

    // Virtual functions with default behaviour
    virtual void analyse();

    // Constructor
    Solver();
};

#endif  // COCO_CORE_SOLVER_HPP_
