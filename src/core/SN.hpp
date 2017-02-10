// CoCo - Supernova templates and simulations package
// Copyright (C) 2017  Szymon Prajs
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

#ifndef COCO_CORE_SN_HPP_
#define COCO_CORE_SN_HPP_

#include <vector>
#include <unordered_map>
#include <string>

#include "Filters.hpp"
#include "Cosmology.hpp"


// Data structure for light curve (one filter per object)
struct LCData {
	std::vector<double> completeMJD_;
    std::vector<double> completeFlux_;
    std::vector<double> completeFluxErr_;

	std::string name_;
    std::string filter_;
    std::vector<double> mjd_;
    std::vector<double> t_;
    std::vector<double> flux_;
    std::vector<double> fluxErr_;

    double mjdMin_;
	double mjdMax_;
	double normalization_;
};


// Data structure for specta
struct SpecData {
    double mjd_;
    std::string file_;

    std::vector<double> rawWav_;
    std::vector<double> rawFlux_;

    std::vector<double> wav_;
    std::vector<double> flux_;
    double fluxNorm_;
};


struct Obs {
    double mjd_;
    double flux_;
    double fluxErr_;

    std::string filter_;
    double wav_;
    double minWav_;
    double maxWav_;
};


// Supernova(SN) class prividing the main data object in the code.
// The class can be initialised multiple ways depending on the usage:
// 1)  SN() - Initialises an empty data structure, used when light curves and
//     spectra are results of simulations and not loaded from text files.
// 2)  SN(string) - Initialises SN class with either a light curve file or
//     list of spectra
// 3)  SN(string,string) - Initialises with both the light curve and list of spectra
class SN {
private:
    // Raw input data for a light curve file
    std::vector<double> _rawMJD;
    std::vector<double> _rawFlux;
    std::vector<double> _rawFluxErr;
    std::vector<std::string> _rawFilter;

public:
    // Data file names
    std::string spectraFileList_;
    std::string lcFile_;

    // SN properties
    double z_;
    std::string name_;
    std::vector<std::string> filterList_;

    // Hash tables mapping filter names onto light curves and MJDs onto spectra
    std::unordered_map<std::string,LCData> lc_;
    std::unordered_map<double,SpecData> spec_;
    std::unordered_map<double,std::vector<Obs>> epoch_;

    // Constructors
    SN();
    SN(std::string);

    // Data loading routines
    void addSpec(std::string,double);
    void saveSpec(double, double scale=1);
    void addEpoch(double);
    void synthesiseLC(const std::vector<std::string>&,std::shared_ptr<Filters>);
    void redshift(double,std::shared_ptr<Cosmology>);
    void loadLC(std::string);
    void setLCStats();
    void restoreCompleteLC();
};


#endif  // COCO_CORE_SN_HPP_
