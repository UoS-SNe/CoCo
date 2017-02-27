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

#ifndef COCO_CORE_FILTERS_HPP_
#define COCO_CORE_FILTERS_HPP_

#include <string>
#include <vector>
#include <unordered_map>


struct FilterData {
    std::string name_;
    std::vector<double> inputWavelength_;
    std::vector<double> inputBandpass_;
    std::vector<double> restWavelength_;
    std::vector<double> wavelength_;
    std::vector<double> bandpass_;
    double area_;
    double zp_;
    double min_;
    double max_;
    double centralWavelength_;
};

class Filters {
private:
    // private data members
    std::string folderPath_;
    std::vector<std::string> fileList_;

    // private functions
    void readFolder();
    void loadFilter(std::string);

public:
    // public data members
    std::unordered_map<std::string, FilterData> filter_;

    // constructor
    Filters(std::string);

    // public functions
    void rescale(double,double,double); // start, end, step
    void rescale(const std::vector<double>&); // wavelength range
    void rescale(double); // step, start and end set automatically

    // Flux for (wavelength, filterName)
    double flux(const std::vector<double>&, const std::string&);
    static std::vector<double> CCM89(const std::vector<double>&,double,double);
};

#endif
