#include "SN.hpp"

#include <iostream>

#include "../vmath/loadtxt.hpp"
#include "../vmath/convert.hpp"

#include "utils.hpp"



// Initialises empty data structure
SN::SN() {}

// Single file initialisation, either spectra or light curve
SN::SN(std::string fileName) {
    // if fileName matches *.dat load as light curve
    if (split(fileName,'.').back() == "dat") {
        loadLC(fileName);

    // if fileName matches *.list load as list of spectra
    } else if (split(fileName,'.').back() == "list") {

    } else {
        std::cout << "Unrecognised data type for file: " << fileName << std::endl;
    }
}

SN::SN(std::string firstName, std::string secondName) {

}


// Load light curve from an input data file
void SN::loadLC(std::string fileName) {
    if (fileExists(fileName)) {
        name_ = baseName(fileName);

        // Load the light curve into a temporarty 2D vector and split into 1D
        std::vector< std::vector<string> > temp;
        vmath::loadtxt<std::string>(fileName, 4, temp);
        _rawMJD = vmath::castString<double>(temp[0]);
        _rawFlux = vmath::castString<double>(temp[1]);
        _rawFluxErr = vmath::castString<double>(temp[2]);
        _rawFilter = temp[3];

        // Make a list of unique filters
        filterList_ = _rawFilter;
        removeDuplicates<std::string>(filterList_);

        // Create data structure for eac
        for (auto &flt : filterList_) {
            LC_[flt].name_ = name_;
            LC_[flt].filter_ = flt;
        }

        // Loop though all data points and assign to correct data vectors
        for (size_t i = 0; i < _rawFilter.size(); ++i) {
            LC_[_rawFilter[i]].completeMJD_.push_back(_rawMJD[i]);
            LC_[_rawFilter[i]].completeFlux_.push_back(_rawFlux[i]);
            LC_[_rawFilter[i]].completeFluxErr_.push_back(_rawFluxErr[i]);
        }

        // Set the working full light curve as the working vrsion
        restoreCompleteLC();
    }

    else {
        std::cout << fileName << " file does not exist" << std::endl;
    }
}


void SN::restoreCompleteLC() {
    for (auto lc : LC_) {
        lc.second.mjd_ = lc.second.completeMJD_;
        lc.second.flux_ = lc.second.completeFlux_;
        lc.second.fluxErr_ = lc.second.completeFluxErr_;
    }
}
