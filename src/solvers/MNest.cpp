#include "MNest.hpp"

#include "multinest.h"

#include "priors.hpp"


MNest::MNest(std::shared_ptr<Model> model) : Solver(model) {}

void MNest::fit() {

}


void MNest::read() {

}
