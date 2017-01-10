#include "Filters.hpp"

#include <math.h>

#include <algorithm>
#include <functional>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>

#include "../vmath/loadtxt.hpp"
#include "../vmath/integrate.hpp"
#include "../vmath/interp.hpp"
#include "../vmath/range.hpp"
#include "../vmath/algebra.hpp"
#include "utils.hpp"


bool FilterData::operator<(const FilterData &rhs) const {
    return (centralWavelength_ < rhs.centralWavelength_);
}


Filters::Filters(std::string path) : folderPath_(path) {
    readFolder();
}


void Filters::readFolder() {
    std::vector<std::string> list = vmath::loadtxt<std::string>(folderPath_ + "/list.txt", 1)[0];

    std::vector<std::string> temp;
    for (size_t i = 0; i < list.size(); ++i) {
        loadFilter(list[i]);
    }
    std::sort(filters_.begin(), filters_.end());

    for (size_t i = 0; i < list.size(); ++i) {
        filterID_[filters_[i].name_] = i;
        filterName_[i] = filters_[i].name_;
    }
}


void Filters::loadFilter(std::string fileName) {
    FilterData filter;
    filter.name_ = split(fileName, '.')[0];

    std::vector< std::vector<double> > data;
    std::string path = folderPath_+ "/" + fileName;
    vmath::loadtxt<double>(path, 2, data);

    double janskyConst = 3631 * 1e-23 * 299792458 * 1e10;
    std::vector<double> waveSq = vmath::mult<double>(data[0], data[0]);
    std::vector<double> jansky = vmath::div<double>(janskyConst, waveSq);
    jansky = vmath::mult<double>(jansky, data[1]);
    double fluxZp = vmath::trapz<double>(jansky, data[0][1] - data[0][0]);

    filter.inputWavelength_ = data[0];
    filter.inputBandpass_ = data[1];
    filter.wavelength_ = filter.inputWavelength_;
    filter.bandpass_ = filter.inputBandpass_;
    filter.restWavelength_ = filter.inputWavelength_;

    filter.area_ = vmath::trapz<double>(data[1], data[0][1] - data[0][0]);
    filter.zp_ = -2.5 * log10(fluxZp / filter.area_);
    filter.centralWavelength_ = vmath::trapz<double>(vmath::mult<double>(data[1], data[0]), data[0][1] - data[0][0]) / filter.area_;

    filter.min_ = -1;
    filter.max_ = -1;
    for (int i = 1; i < (filter.bandpass_.size() - 1); i++) {
        if (filter.bandpass_[i] != 0 and filter.min_ == -1) {
            filter.min_ = filter.wavelength_[i-1];
        }
        if (filter.bandpass_[filter.bandpass_.size() - 1 - i] != 0 and filter.max_ == -1) {
            filter.max_ = filter.wavelength_[filter.bandpass_.size() - i];
        }
        if (filter.min_ != -1 && filter.max_ != -1) break;
    }

    filters_.push_back(filter);
}


void Filters::rescale(const std::vector<double> &wavelength) {
    for (int i = 0; i < filters_.size(); ++i) {
        filters_[i].wavelength_ = wavelength;
        filters_[i].bandpass_ = vmath::interp<double>(wavelength,filters_[i].inputWavelength_,filters_[i].inputBandpass_);
    }
}


void Filters::rescale(double start, double end, double step) {
    std::vector<double> wavelength = vmath::range<double>(start, end, step);
    for (int i = 0; i < filters_.size(); ++i) {
        filters_[i].wavelength_ = wavelength;
        filters_[i].bandpass_ = vmath::interp<double>(wavelength,filters_[i].inputWavelength_,filters_[i].inputBandpass_);
    }
}

void Filters::rescale(double step) {
    double start, end;
    for (int i = 0; i < filters_.size(); ++i) {
        start = filters_[i].inputWavelength_.front();
        start -= fmod(start, step);
        end = filters_[i].inputWavelength_.back();
        end -= fmod(end, step);
        end += step;

        filters_[i].wavelength_ = vmath::range<double>(start, end, step);
        filters_[i].bandpass_ = vmath::interp<double>(filters_[i].wavelength_,filters_[i].inputWavelength_,filters_[i].inputBandpass_);
    }
}

double Filters::flux(const std::vector<double>& SED, const std::string& filterName) {
    if (filterID_.find(filterName) == filterID_.end()) {
        return 0;

    } else {
        int ID = filterID_.at(filterName);
        std::vector<double> filteredSED = vmath::mult<double>(SED, filters_[ID].bandpass_);
        double integFlux = vmath::trapz<double>(filteredSED, filters_[ID].wavelength_[1] - filters_[ID].wavelength_[0]);
        return integFlux / filters_[ID].area_;
    }
}
