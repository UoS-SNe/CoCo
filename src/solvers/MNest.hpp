#ifndef COCO_SOLVERS_MNEST_HPP_
#define COCO_SOLVERS_MNEST_HPP_

#include "MultiNest.h"

#include "../core/Solver.hpp"
#include "../core/Model.hpp"


class MNest : public Solver {
public:
    // MultiNest specific temporary data containers
    std::string rootPath;

    // Overrides for solver specific methods
    void fit();
    void read();

    // Constructor
    MNest(std::shared_ptr<Model>);
};


#endif  // COCO_SOLVERS_MNEST_HPP_
