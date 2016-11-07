#include "LC.hpp"

using namespace std;
using namespace vmath;


LC::LC() {}


LC::LC(string file) {
    set(file);
}


void LC::set(string file) {
    readData(file);
    setFilterList();
    makeDataLists();
    normalizeDataLists();
}


void LC::readData(string file) {
    name_ = baseName(file);
    vector< vector<string> > data;
    loadtxt<string>(file, 4, data);

    completeMJD_ = castString<double>(data[0]);
    completeFlux_ = castString<double>(data[1]);
    completeFluxErr_ = castString<double>(data[2]);
    completeFilter_ = data[3];

    restoreCompleteLC();
}


void LC::restoreCompleteLC() {
	mjd_ = completeMJD_;
    flux_ = completeFlux_;
    fluxErr_ = completeFluxErr_;
    filter_ = completeFilter_;
}


void LC::setFilterList() {
    filterList_ = filter_;
    removeDuplicates<string>(filterList_);
}


// TODO - Test for real data
void LC::removeData(double start, double end) {
    vector<double>::iterator mjd_it = mjd_.begin();
    vector<double>::iterator flux_it = flux_.begin();
    vector<double>::iterator fluxErr_it = fluxErr_.begin();
    vector<string>::iterator filter_it = filter_.begin();

    for (; mjd_it != mjd_.end(); ) {
        if (*mjd_it < start || *mjd_it > end) {
            mjd_it = mjd_.erase(mjd_it);
            flux_it = flux_.erase(flux_it);
            fluxErr_it = fluxErr_.erase(fluxErr_it);
            filter_it = filter_.erase(filter_it);

        } else {
            mjd_it++;
            flux_it++;
            fluxErr_it++;
            filter_it++;
        }
    }
}


void LC::removeData(string filter) {
    vector<double>::iterator mjd_it = mjd_.begin();
    vector<double>::iterator flux_it = flux_.begin();
    vector<double>::iterator fluxErr_it = fluxErr_.begin();
    vector<string>::iterator filter_it = filter_.begin();

    for (; filter_it != filter_.end(); ) {
        if (*filter_it == filter) {
            mjd_it = mjd_.erase(mjd_it);
            flux_it = flux_.erase(flux_it);
            fluxErr_it = fluxErr_.erase(fluxErr_it);
            filter_it = filter_.erase(filter_it);

        } else {
            mjd_it++;
            flux_it++;
            fluxErr_it++;
            filter_it++;
        }
    }
}


void LC::makeDataLists() {
    mjdList_.resize(filterList_.size());
    tList_.resize(filterList_.size());
    fluxList_.resize(filterList_.size());
    fluxErrList_.resize(filterList_.size());
    mjdMinList_.resize(filterList_.size());
    mjdMaxList_.resize(filterList_.size());

    for (size_t j = 0; j < filterList_.size(); ++j) {
        mjdList_[j].clear();
        fluxList_[j].clear();
        fluxErrList_[j].clear();

        for (size_t i = 0; i < filter_.size(); ++i) {
            if (filter_[i] == filterList_[j]) {
                mjdList_[j].push_back(mjd_[i]);
                fluxList_[j].push_back(flux_[i]);
                fluxErrList_[j].push_back(fluxErr_[i]);
            }
        }

        mjdMaxList_[j] = max<double>(mjdList_[j]);
        mjdMinList_[j] = min<double>(mjdList_[j]);

        tList_[j] = mjdList_[j];
        tList_[j] = sub<double>(tList_[j], mjdMinList_[j]);
    }

    mjdMax_ = max<double>(mjdMaxList_);
    mjdMin_ = min<double>(mjdMinList_);
}


void LC::normalizeDataLists() {
    normalization_.resize(filterList_.size());

    for (size_t j = 0; j < filterList_.size(); ++j) {
        normalization_[j] = max<double>(fluxList_[j]);

        fluxList_[j] = div<double>(fluxList_[j], normalization_[j]);
        fluxErrList_[j] = div<double>(fluxErrList_[j], normalization_[j]);
    }
}
