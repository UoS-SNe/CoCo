// CoCo - Supernova templates and simulations package
// Copyright (C) 2016, 2017  Szymon Prajs
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

#include <iostream>
#include <vector>
#include <string>

#include "vmath/algebra.hpp"
#include "vmath/convert.hpp"
#include "vmath/loadtxt.hpp"
#include "vmath/range.hpp"
#include "vmath/stat.hpp"

#include "core/Cosmology.hpp"
#include "core/Filters.hpp"
#include "core/SN.hpp"
#include "core/utils.hpp"
#include "models/Karpenka12.hpp"
#include "solvers/MPFitter.hpp"

struct Workspace {
    // Code inputs
    std::string zeroFilter_;
    std::string inputLCList_;

    // Filter info
    std::string filterPath_;
    std::shared_ptr<Filters> filters_;

    // Cosmology
    std::shared_ptr<Cosmology> cosmology_;

    // SN data
    std::vector<std::string> lcList_;
    std::vector<double> z_;
    std::unordered_map<std::string, SN> sn_;
};


void help() {
    std::cout << "CoCo - SpecPhase: \n";
    std::cout << "Originally writen by Natasha Karpenka, ";
    std::cout << "currently maintained by Szymon Prajs (S.Prajs@soton.ac.uk) ";
    std::cout << "and Rob Firth.\n";
    std::cout << "\nUsage:\n";
    std::cout << "specphase filter_name\n";
    std::cout << std::endl;
}


// Assign input options to workspace parameters
void applyOptions(std::vector<std::string> &options, std::shared_ptr<Workspace> w) {
    if (options.size() < 1 || options[0] == "-h" || options[0] == "--help") {
        help();
        exit(0);

    } else if (options.size() == 2)  {
        w->inputLCList_ = options[0];
        w->zeroFilter_ = options[1];

        if (utils::fileExists(w->inputLCList_)) {
            std::vector< std::vector<std::string> > temp;
            vmath::loadtxt<std::string>(w->inputLCList_, 2, temp);
            w->lcList_ = temp[0];
            vmath::castString<double>(temp[1], w->z_);
        }

    } else if (options.size() == 3) {
            w->lcList_ = {options[1]};
            w->z_ = {atof(options[2].c_str())};

    } else {
        std::cout << "Options are not currently implemented\n";
        std::cout << "Program will continue executing" << std::endl;
    }
}


// Automatically fill in all unassigned properties with defaults
void fillUnassigned(std::shared_ptr<Workspace> w) {
    // Do a sanity check for the LC files
    if (w->lcList_.size() == 0) {
        std::cout << "Something went seriously wrong.";
        std::cout << "Please consider report this bug on our project GitHub page";
        std::cout << std::endl;
        exit(0);
    }

	// Load the light curves
	for (size_t i = 0; i < w->lcList_.size(); ++i) {
		if (utils::fileExists("recon/" + w->lcList_[i] + ".dat")) {
            SN sn;
            sn.name_ = utils::baseName(w->lcList_[i]);
            sn.z_ = w->z_[i];
			w->sn_[sn.name_] = sn;
		}
	}
}


// Scan recon folder for mangled spectra and assign to the correct SN
void scanRecon(std::shared_ptr<Workspace> w) {
    std::vector<std::string> files;
    utils::dirlist("recon", files);

    std::string snname;
    double mjd;
    for (auto &file : files) {
        if (utils::fileExtention(file) == "spec") {
            snname = utils::split(utils::baseName(file), '_').front();
            mjd = atof(utils::split(utils::baseName(file), '_').back().c_str());
            w->sn_[snname].addSpec("recon/" + file, mjd);
        }
    }
}


void makeSyntheticLC(std::shared_ptr<Workspace> w) {
    for (auto &sn : w->sn_) {
        sn.second.redshift(0, w->cosmology_);
        sn.second.synthesiseLC({w->zeroFilter_}, w->filters_);
    }
}


void fitPhase(std::shared_ptr<Workspace> w) {
    for (auto &sn : w->sn_) {
        ofstream phaseFile;
        phaseFile.open("recon/" + sn.second.name_ + ".phase");

        auto lc = sn.second.lc_[w->zeroFilter_];

        // Initialise the model
        std::shared_ptr<Karpenka12> karpenka12(new Karpenka12);
        karpenka12->x_ = vmath::sub<double>(lc.mjd_, lc.mjdMin_);
        karpenka12->y_ = vmath::div<double>(lc.flux_, lc.normalization_);
        karpenka12->sigma_ = std::vector<double>(lc.flux_.size(), 1);
        std::shared_ptr<Model> model = dynamic_pointer_cast<Model>(karpenka12);

        // Initialise solver
        MPFitter solver(model);
        solver.xRecon_ = vmath::range<double>(-15, lc.mjdMax_ - lc.mjdMin_ + 20, 1);

        // Perform fitting
        solver.analyse();

        size_t indexMax = std::distance(solver.bestFit_.begin(),
                                        max_element(solver.bestFit_.begin(),
                                                    solver.bestFit_.end()));
        double mjdZeroPhase = solver.xRecon_[indexMax] + lc.mjdMin_;

        for (auto &spec : sn.second.spec_) {
            phaseFile << "spectra/" << sn.second.name_ << "_" << spec.second.mjd_;
            phaseFile << " " << (spec.second.mjd_ - mjdZeroPhase) / (1.0 + sn.second.zRaw_) << "\n";
        }

        phaseFile.close();

        sn.second.saveSpec(mjdZeroPhase);
    }
}


int main (int argc, char* argv[]) {
    std::vector<std::string> options;
    std::shared_ptr<Workspace> w(new Workspace());

    utils::getArgv(argc, argv, options);
    applyOptions(options, w);
    fillUnassigned(w);

    // Read in filters and find the ID of the filter used to determine the phase
    w->filterPath_ = "data/filters";
    w->filters_ = std::shared_ptr<Filters>(new Filters(w->filterPath_));
    w->cosmology_ = std::shared_ptr<Cosmology>(new Cosmology(0));

    // run SpecPhase pipeline
    scanRecon(w);
    makeSyntheticLC(w);
    fitPhase(w);

    return 0;
}
