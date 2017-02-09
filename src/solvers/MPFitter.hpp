#ifndef COCO_SOLVERS_MPFITTER_HPP_
#define COCO_SOLVERS_MPFITTER_HPP_

#include "mpfit.h"

#include "../core/Solver.hpp"
#include "../core/Model.hpp"


class MPFitter : public Solver {
public:
    // MPFitter specific temporary data containers
    int status;
    mp_result result;
    mp_config config;
    mp_par *pars;
    std::vector<double> par;
    std::vector<double> parErr;

    // Static function for mpfit.c
    static int resFunc(int,int,double*,double*,double**,void*);

    // Overrides for solver specific methods
    void fit();
    void read();
    void stats();

    // Constructor
    MPFitter(std::shared_ptr<Model>);
};


#endif  // COCO_SOLVERS_MPFITTER_HPP_
