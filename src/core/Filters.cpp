#include "Filters.hpp"

#include <math.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <fstream>
#include <iostream>

#include "../vmath/loadtxt.hpp"
#include "../vmath/integrate.hpp"
#include "../vmath/interp.hpp"
#include "../vmath/range.hpp"
#include "../vmath/algebra.hpp"

#include "utils.hpp"


Filters::Filters(std::string path) : folderPath_(path) {
    readFolder();
}


void Filters::readFolder() {
    std::vector<std::string> list = vmath::loadtxt<std::string>(folderPath_ + "/list.txt", 1)[0];

    for (size_t i = 0; i < list.size(); ++i) {
        loadFilter(list[i]);
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

    filter_.insert(std::make_pair(filter.name_, filter));
}


void Filters::rescale(const std::vector<double> &wavelength) {
    for (auto flt : filter_) {
        flt.second.wavelength_ = wavelength;
        flt.second.bandpass_ = vmath::interp<double>(wavelength, flt.second.inputWavelength_, flt.second.inputBandpass_);
    }
}


void Filters::rescale(double start, double end, double step) {
    std::vector<double> wavelength = vmath::range<double>(start, end, step);
    for (auto flt : filter_) {
        flt.second.wavelength_ = wavelength;
        flt.second.bandpass_ = vmath::interp<double>(wavelength, flt.second.inputWavelength_, flt.second.inputBandpass_);
    }
}

void Filters::rescale(double step) {
    double start, end;
    for (auto flt : filter_) {
        start = flt.second.inputWavelength_.front();
        start -= fmod(start, step);
        end = flt.second.inputWavelength_.back();
        end -= fmod(end, step);
        end += step;

        flt.second.wavelength_ = vmath::range<double>(start, end, step);
        flt.second.bandpass_ = vmath::interp<double>(flt.second.wavelength_, flt.second.inputWavelength_, flt.second.inputBandpass_);
    }
}

double Filters::flux(const std::vector<double>& SED, const std::string& filterName) {
    if (filter_.find(filterName) == filter_.end()) {
        return 0;

    } else {
        std::vector<double> filteredSED = vmath::mult<double>(SED, filter_[filterName].bandpass_);
        double integFlux = vmath::trapz<double>(filteredSED, filter_[filterName].wavelength_[1] - filter_[filterName].wavelength_[0]);
        return integFlux / filter_[filterName].area_;
    }
}
