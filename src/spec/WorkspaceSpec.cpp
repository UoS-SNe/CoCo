 #include "WorkspaceSpec.hpp"

using namespace std;


WorkspaceSpec::WorkspaceSpec() {
    // Initialise all variables with defaults designed to halt the code if empty
    SpecListFile_ = "None";
    filterPath_ = "data/filters";

    filterList_.resize(0);
}


void WorkspaceSpec::lcSlice() {
    int nearIndex = 0;
    vector<string> filterTemp;
    int fIndex;

    double specWavMin = 0;
    double specWavMax = 0;

    // make a slice for every spectum file
    for (size_t i = 0; i < SNe_.size(); ++i) {
        SNe_[i].lcFilters_.clear();
        SNe_[i].lcFlux_.clear();
        SNe_[i].lcFluxError_.clear();
        SNe_[i].lcCentralWavelength_.clear();

        // Find the wavelength coverage of the
        specWavMin = min<double>(SNe_[i].wav_);
        specWavMax = max<double>(SNe_[i].wav_);

        // Sort filters according to their central wavelength (core/Filters sorting)
        filterTemp = SNe_[i].lc_.filterList_;
        for (auto filter : filters_->filter_) {
            auto it = find(filterTemp.begin(), filterTemp.end(), filter.second.name_);

            if (it != filterTemp.end()) {
                fIndex = distance(filterTemp.begin(), it);

                if (specWavMin < min<double>(filter.second.wavelength_) &&
                specWavMax > max<double>(filter.second.wavelength_)) {

                    // find the lightcurve point closest to the spectrum
                    nearIndex = nearest<double>(SNe_[i].lc_.mjdList_[fIndex], SNe_[i].mjd_);
                    SNe_[i].lcFlux_.push_back(SNe_[i].lc_.fluxList_[fIndex][nearIndex]);
                    SNe_[i].lcFluxError_.push_back(SNe_[i].lc_.fluxErrList_[fIndex][nearIndex]);
                    SNe_[i].lcFilters_.push_back(filterTemp[fIndex]);
                    SNe_[i].lcCentralWavelength_.push_back(filter.second.centralWavelength_);
                }
            }
        }

        // Normalise light curve slices for mangling
        SNe_[i].normFlux_ = accumulate(SNe_[i].lcFlux_.begin(), SNe_[i].lcFlux_.end(), 0.0) / SNe_[i].lcFlux_.size();
        SNe_[i].lcFlux_ = div<double>(SNe_[i].lcFlux_, SNe_[i].normFlux_);
        SNe_[i].lcFluxError_ = div<double>(SNe_[i].lcFluxError_, SNe_[i].normFlux_);
    }
}
