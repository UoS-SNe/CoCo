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
    vector<string> filterTemp;
    int fIndex;

    // make a slice for every spectum file
    for (size_t i = 0; i < SNe_.size(); ++i) {
        SNe_[i].lcFilters_.clear();
        SNe_[i].lcFlux_.clear();
        SNe_[i].lcFluxError_.clear();
        SNe_[i].lcCentralWavelength_.clear();
        
        // Sort filters according to their central wavelength (core/Filters sorting)
        filterTemp = SNe_[i].lc_.filterList_;
        for (auto filter : filters_->filters_) {
            auto it = find(filterTemp.begin(), filterTemp.end(), filter.name_);

            if (it != filterTemp.end()) {
                fIndex = distance(filterTemp.begin(), it);

                // find the lightcurve point closest to the spectrum
                minIndex = nearest<double>(SNe_[i].lc_.mjdList_[fIndex], SNe_[i].mjd_);
                SNe_[i].lcFlux_.push_back(SNe_[i].lc_.fluxList_[fIndex][minIndex]);
                SNe_[i].lcFluxError_.push_back(SNe_[i].lc_.fluxErrList_[fIndex][minIndex]);
                SNe_[i].lcFilters_.push_back(filterTemp[fIndex]);
                SNe_[i].lcCentralWavelength_.push_back(filter.centralWavelength_);
            }
        }
    }
}
