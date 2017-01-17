#include <iostream>

#include "core/Cosmology.hpp"
#include "core/Filters.hpp"
#include "core/SN.hpp"
#include "core/Solver.hpp"
#include "core/Model.hpp"
#include "solvers/MNest.hpp"
#include "models/Firth17.hpp"


int main(int argc, char *argv[]) {
    Cosmology cosmology(0.2);
    Filters filter("data/filters/list.txt");
    SN sn;
    std::shared_ptr<Model> model(new Firth17);
    MNest solver(model);

    return 0;
}
