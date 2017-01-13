#ifndef COCO_CORE_SOLVER_HPP_
#define COCO_CORE_SOLVER_HPP_

#include <vector>
#include <string>

class Solver {
public:
    int noParams_;
    std::vector<double> fitParams_;
    std::vector<double> startingParams_;
    std::vector<double> lParams_;
    std::vector<double> uParams_;
    std::vector<std::string> paramNames_;

    // Pure virtual functions that must be overwriten by solver implementations
    virtual void fit() = 0;
    virtual void read() = 0;

    // Virtual functions with default behaviour
    virtual void analyse();

    // Constructor
    Solver();
};

#endif  // COCO_CORE_SOLVER_HPP_
