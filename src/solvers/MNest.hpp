#ifndef COCO_SOLVERS_MNEST_HPP_
#define COCO_SOLVERS_MNEST_HPP_

#include "../core/Solver.hpp"
#include "MultiNest.h"

class MNest : public Solver {
public:
    // MultiNest specific temporary data containers

    // Overrides for solver specific methods
    void fit();
    void read();
};


#endif  // COCO_SOLVERS_MNEST_HPP_
