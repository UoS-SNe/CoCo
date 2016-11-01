#include "Solver.hpp"

Solver::Solver(shared_ptr<Workspace> &w) {
    w_ = w;

    mean = vector<double>(w_->model_.npar_, 0);
    sigma = vector<double>(w_->model_.npar_, 0);
    median = vector<double>(w_->model_.npar_, 0);
    median_sigma = vector<double>(w_->model_.npar_, 0);
}
