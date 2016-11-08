#include "Workspace.hpp"

using namespace std;


Workspace::Workspace() {
    // Initialise all variables with defaults designed to halt the code if empty
    SpecListFile_ = "None";
    filterPath_ = "data/filters";

    filterList_.resize(0);
}


void Workspace::lcSlice() {
    int minIndex = 0;

    // make a slice for every spectum file
    for (size_t i = 0; i < SNe_.size(); ++i) {
        SNe_[i].lcFilters_ = SNe_[i].lc_.filter_;
        SNe_[i].lcFlux_.resize(SNe_[i].lcFilters_.size());
        SNe_[i].lcFluxError_.resize(SNe_[i].lcFilters_.size());

        // find the lightcurve point closest to the spectrum
        for (size_t j = 0; j < SNe_[i].lcFilters_.size(); ++j) {
            minIndex = nearest<double>(SNe_[i].lc_.mjdList_[j], SNe_[i].mjd_);
            SNe_[i].lcFlux_[j] = SNe_[i].lc_.fluxList_[j][minIndex];
            SNe_[i].lcFluxError_[j] = SNe_[i].lc_.fluxErrList_[j][minIndex];
        }
    }
}
