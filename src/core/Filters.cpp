// CoCo - Supernova templates and simulations package
// Copyright (C) 2014, 2016, 2017  Szymon Prajs
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Contact author: S.Prajs@soton.ac.uk

#include "Filters.hpp"

#include <math.h>

#include <iostream>

#include "../vmath/loadtxt.hpp"
#include "../vmath/integrate.hpp"
#include "../vmath/interp.hpp"
#include "../vmath/range.hpp"
#include "../vmath/algebra.hpp"
#include "../vmath/stat.hpp"

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


// Load filters responses and calculate key properties
void Filters::loadFilter(std::string fileName) {
    FilterData filter;
    filter.name_ = utils::split(fileName, '.')[0];

    // Load a filter text file to a vector
    std::vector< std::vector<double> > data;
    std::string path = folderPath_+ "/" + fileName;
    vmath::loadtxt<double>(path, 2, data);

    // Truncate filters at the 3% level TODO double check rationale
    double truncLimit = 0.03 * vmath::max(data[1]);
    for (int i = data[0].size() - 1; i >= 0; --i) {
        if (data[1][i] < truncLimit) {
            data[0].erase(data[0].begin() + i);
            data[1].erase(data[1].begin() + i);

        }
    }
    data[0].insert(data[0].begin(), vmath::min(data[0]) - (data[0][1] - data[0][0]));
    data[1].insert(data[1].begin(), 0);
    data[0].push_back(vmath::max(data[0]) + (data[0][1] - data[0][0]));
    data[1].push_back(0);

    // Calculate the zero point of a filter
    constexpr double janskyConst = 3631 * 1e-23 * 299792458 * 1e10;
    std::vector<double> waveSq = vmath::mult<double>(data[0], data[0]);
    std::vector<double> jansky = vmath::div<double>(janskyConst, waveSq);
    jansky = vmath::mult<double>(jansky, data[1]);
    double fluxZp = vmath::trapz<double>(jansky, data[0][1] - data[0][0]);

    // Assign properties to the a FilerData object
    filter.inputWavelength_ = data[0];
    filter.inputBandpass_ = data[1];
    filter.wavelength_ = filter.inputWavelength_;
    filter.bandpass_ = filter.inputBandpass_;
    filter.restWavelength_ = filter.inputWavelength_;
    filter.area_ = vmath::trapz<double>(data[1], data[0][1] - data[0][0]);
    filter.zp_ = -2.5 * log10(fluxZp / filter.area_);
    filter.centralWavelength_ = vmath::trapz<double>(vmath::mult<double>(data[1], data[0]), data[0][1] - data[0][0]) / filter.area_;

    // Find the wavelength span of the filter
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
    for (auto &flt : filter_) {
        flt.second.wavelength_ = wavelength;
        flt.second.bandpass_ = vmath::interp<double>(wavelength, flt.second.inputWavelength_, flt.second.inputBandpass_);
    }
}


void Filters::rescale(double start, double end, double step) {
    std::vector<double> wavelength = vmath::range<double>(start, end, step);
    for (auto &flt : filter_) {
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
        double integFlux = vmath::trapz<double>(filteredSED, filter_[filterName].wavelength_);
        return integFlux / filter_[filterName].area_;
    }
}

std::vector<double> Filters::CCM89(const std::vector<double>& wav, double Eb_v, double r_v) {
    // Cardelli'89 extinction coefficients
    const std::vector<double> ccm89CoeffsA = {1., 0.17699, -0.50447, -0.02427,
                                              0.72085, 0.01979, -0.77530, 0.32999};

    const std::vector<double> ccm89CoeffsB = {0., 1.41338, 2.28305, 1.07233,
                                              -5.38434, -0.62251, 5.30260, -2.09002};

    std::vector<double> res(wav.size(), 1);
    std::vector<double> x(wav.size(), 1e4);
    x = vmath::div(x, wav);

    double a, b, y, y2, y3, yn;

    for (size_t i = 0; i < wav.size(); ++i) {
        if (x[i] < 1.1) {
            y = pow(x[i], 1.61);
            a = 0.574 * y;
            b = -0.527 * y;

        } else if (x[i] < 3.3) {
            y = x[i] - 1.82;
            a = ccm89CoeffsA[0];
            b = ccm89CoeffsB[0];
            yn = 1.;
            for (size_t j = 1; j < 8; ++j) {
                yn *= y;
                a += ccm89CoeffsA[j] * yn;
                b += ccm89CoeffsB[j] * yn;
            }

        } else if (x[i] < 8.0) {
            y = x[i] - 4.67;
            a = 1.752 - 0.316*x[i] - (0.104 / (y*y + 0.341));
            y = x[i] - 4.62;
            b = -3.090 + 1.825*x[i] + (1.206 / (y*y + 0.263));

            if (x[i] > 5.9) {
                y = x[i] - 5.9;
                y2 = y * y;
                y3 = y2 * y;
                a += -0.04473*y2 - 0.009779*y3;
                b += 0.2130*y2 + 0.1207*y3;
            }

        } else {
            y = x[i] - 8.0;
            y2 = y * y;
            y3 = y2 * y;
            a = -0.070*y3 + 0.137*y2 - 0.628*y - 1.073;
            b = 0.374*y3 - 0.420*y2 + 4.257*y + 13.670;
        }

        res[i] = Eb_v * (a * r_v + b);
        res[i] = pow(10, 0.4*res[i]);
    }

    return res;
}
