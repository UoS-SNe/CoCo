#include "Solver.hpp"

#include <iostream>


// Constructor
Solver::Solver(std::shared_ptr<Model> model) : model_(model) {}


void Solver::analyse() {
    fit();
    read();
}
