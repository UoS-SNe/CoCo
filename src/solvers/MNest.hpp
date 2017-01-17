#ifndef COCO_SOLVERS_MNEST_HPP_
#define COCO_SOLVERS_MNEST_HPP_

#include "multinest.h"

#include "../core/Solver.hpp"
#include "../core/Model.hpp"


class MNest : public Solver {
public:
    // MultiNest specific temporary data containers
    std::string rootPath;

    // static functions used by MultiNest
    static void dumper(int&,int&,int&,double**,double**,double**,double&,
                       double&,double&,void*);
    static void logLike(double*,int&,int&,double&,void*);

    // Overrides for solver specific methods
    void fit();
    void read();
    void stats();

    // Constructor
    MNest(std::shared_ptr<Model>);
};


#endif  // COCO_SOLVERS_MNEST_HPP_
