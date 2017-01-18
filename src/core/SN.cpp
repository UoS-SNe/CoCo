#include "SN.hpp"

#include <math.h>

#include <iostream>

#include "../vmath/algebra.hpp"
#include "../vmath/convert.hpp"
#include "../vmath/loadtxt.hpp"
#include "../vmath/stat.hpp"

#include "utils.hpp"



// Initialises empty data structure
SN::SN() {}

// Single file initialisation, either spectra or light curve
SN::SN(std::string fileName) {
    // if fileName matches *.dat load as light curve
    if (utils::fileExtention(fileName) == "dat") {
        loadLC(fileName);

    } else {
        std::cout << "Unrecognised data type for file: " << fileName << std::endl;
    }
}


// Add a spectrum file and read the data
void SN::addSpec(std::string fileName, double mjd) {
    if (utils::fileExists(fileName)) {
        // Create a temporarty SpecData object
        SpecData sd;

        // Load data into a temporarty vector then assign to SpecData object
        std::vector< std::vector<double> > temp = vmath::loadtxt<double>(fileName, 2);
        sd.rawWav_ = temp[0];
        sd.rawFlux_ = temp[1];
        sd.wav_ = sd.rawWav_;
        sd.flux_ = sd.rawFlux_;
        sd.mjd_ = mjd;

        // Add the data object to the spec_ unordered_map accessed by MJD
        spec_[mjd] = sd;
    }
}


// Create a slice though the light curves at a given MJD
void SN::addEpoch(double mjd) {
    // Check if any light curves are assigned
    if (!lc_.empty()) {
        // create a temporarty SNEpoch object
        SNEpoch epoch;

        size_t idxNearest;
        for (auto &lc : lc_) {
            epoch.mjd_ = mjd;
            idxNearest = vmath::nearest(lc.second.mjd_, mjd);

            // Check if the data points is within one day from the spectrum
            if (!(fabs(lc.second.mjd_[idxNearest] - mjd) > 1)) {
                epoch.flux_.push_back(lc.second.flux_[idxNearest]);
                epoch.fluxErr_.push_back(lc.second.fluxErr_[idxNearest]);
                epoch.filter_.push_back(lc.second.filter_);
            }
        }

        // Add the light curve slice to epoch_ unordered_map accessed by MJD
        epoch_[mjd] = epoch;
    }
}


// Load light curve from an input data file
void SN::loadLC(std::string fileName) {
    if (utils::fileExists(fileName)) {
        name_ = utils::baseName(fileName);

        // Load the light curve into a temporarty 2D vector and split into 1D
        std::vector< std::vector<string> > temp;
        vmath::loadtxt<std::string>(fileName, 4, temp);
        _rawMJD = vmath::castString<double>(temp[0]);
        _rawFlux = vmath::castString<double>(temp[1]);
        _rawFluxErr = vmath::castString<double>(temp[2]);
        _rawFilter = temp[3];

        // Make a list of unique filters
        filterList_ = _rawFilter;
        utils::removeDuplicates<std::string>(filterList_);

        // Create data structure for each light curve. One per filter
        for (auto &flt : filterList_) {
            lc_[flt].name_ = name_;
            lc_[flt].filter_ = flt;
        }

        // Loop though all data points and assign to correct data vectors
        for (size_t i = 0; i < _rawFilter.size(); ++i) {
            lc_[_rawFilter[i]].completeMJD_.push_back(_rawMJD[i]);
            lc_[_rawFilter[i]].completeFlux_.push_back(_rawFlux[i]);
            lc_[_rawFilter[i]].completeFluxErr_.push_back(_rawFluxErr[i]);
        }

        // Set the full light curve as the working version
        restoreCompleteLC();
    }

    else {
        std::cout << fileName << " file does not exist" << std::endl;
    }
}


void SN::restoreCompleteLC() {
    for (auto &lc : lc_) {
        lc.second.mjd_ = lc.second.completeMJD_;
        lc.second.flux_ = lc.second.completeFlux_;
        lc.second.fluxErr_ = lc.second.completeFluxErr_;

        lc.second.mjdMin_ = vmath::min<double>(lc.second.mjd_);
        lc.second.mjdMax_ = vmath::max<double>(lc.second.mjd_);
        lc.second.normalization_ = vmath::max<double>(lc.second.flux_);

        lc.second.t_ = vmath::sub<double>(lc.second.mjd_, lc.second.mjdMin_);
    }
}
