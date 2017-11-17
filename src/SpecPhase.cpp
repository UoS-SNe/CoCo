// CoCo - Supernova templates and simulations package
// Copyright (C) 2016, 2017  Szymon Prajs, Robert Firth
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
// Contact author: S.Prajs@soton.ac.uk; R.E.Firth@soton.ac.uk

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
#include "models/Bazin09.hpp"
#include "models/Kessler10.hpp"
#include "models/Karpenka12.hpp"
#include "models/Karpenka12Afterglow.hpp"
#include "models/Firth17Complex.hpp"
#include "solvers/MNest.hpp"


struct Workspace {
    // Code inputs
    std::string zeroFilter_;
    std::string inputLCList_;
    std::string modelWanted_;

    // Filter info
    std::string filterPath_;
    std::shared_ptr<Filters> filters_;

    // Cosmology
    std::shared_ptr<Cosmology> cosmology_;

    // SN data
    std::vector<std::string> lcList_;
    std::vector<double> z_;
    std::vector<double> distMod_;
    std::unordered_map<std::string, SN> sn_;
};


void help() {
    std::cout << "CoCo - SpecPhase: \n";
    std::cout << "Originally writen by Natasha Karpenka, ";
    std::cout << "currently maintained by Szymon Prajs (S.Prajs@soton.ac.uk) ";
    std::cout << "and Rob Firth (R.E.Firth@soton.ac.uk).\n";
    std::cout << "\nUsage:\n";
    std::cout << "specphase filter_name\n";
    std::cout << "\n";
    std::cout << "----------------------------------------------------------\n";
    std::cout << "Options:\n";
    std::cout << "-m, -model <model_name>\n";
    std::cout << std::endl;
}


// Helper function to search for filter in the listfile
bool in_array(const std::string &value, const std::vector<string> &array){
//    https://stackoverflow.com/a/20303915
    return std::find(array.begin(), array.end(), value) != array.end();
}

// Assign input options to workspace parameters
void applyOptions(std::vector<std::string> &options, std::shared_ptr<Workspace> w) {
    std::cout << options.size() << std::endl;

    // declare command string instance to store parsed args
    std::vector<std::string> command;

    // initialise list of filters to match against
    std::vector<std::string> filterArray = vmath::loadtxt<std::string>(w->filterPath_ + "/list.txt", 1)[0];

    if (options.size() < 1 || options[0] == "-h" || options[0] == "--help") {
        help();
        exit(0);
    }

    for (size_t i = 0; i < options.size(); ++i) {

        std::cout << i << "\t" << options[i] << std::endl;


        if (options[i].length() > 5){
            if(options[i].substr( options[i].length() - 5 ) == ".list"){

                std::cout << options[i]  << " looks like a list file" << std::endl;
                w->inputLCList_ = options[i];

                if (utils::fileExists(w->inputLCList_)) {
                    std::vector< std::vector<std::string> > temp;
                    vmath::loadtxt<std::string>(w->inputLCList_, 3, temp);
                    w->lcList_ = temp[0];
                    vmath::castString<double>(temp[1], w->z_);
                    vmath::castString<double>(temp[2], w->distMod_);
                 }
            }
        }

        if (in_array(options[i]+ ".dat", filterArray)){

            std::cout << options[i]  << " looks like a filter" << std::endl;
            w->zeroFilter_ = options[i];

        }

        if (options.size() > 2 && i > 1){
            std::cout << "more than 2 args" << std::endl;
                if (options[i] == "-m"||
                   options[i] == "--mod" ||
                   options[i] == "--model" ) {
                    std::cout << "using model " << std::endl;
                    if (i+1 < options.size()) {
                        command = {options[i], options[i+1]};
                        std::cout << options[i] << " " << options[i+1] << std::endl;
                        i++;  // skip the next option once the previous is assigned

                        w->modelWanted_ = options[i]; // add model to workspace
                    } else {
                        std::cout << "You need to pass a model" << std::endl;
                    }
                } else {
                    std::cout << options[i] << " is not a valid flag" << std::endl;
                }
         }

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
            sn.distMod_ = w->distMod_[i];
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
        sn.second.redshift(0, w->cosmology_, false);
        sn.second.scaleSpectra(pow(10, 0.4 * sn.second.distMod_));
        sn.second.synthesiseLC({w->zeroFilter_}, w->filters_);
    }
}


void fitPhase(std::shared_ptr<Workspace> w) {
    for (auto &sn : w->sn_) {
        std::ofstream phaseFile;
        phaseFile.open("recon/" + sn.second.name_ + ".phase"); // Open file for output

        auto lc = sn.second.lc_[w->zeroFilter_];

        // Initialise the model
        std::cout << "Fitting SN:\t" << sn.second.name_ << std::endl;
        std::cout << "Relative to filter:\t" << lc.filter_ << std::endl;
        std::cout << "With model:\t" << w->modelWanted_ << std::endl;  // Check the passed args

        std::shared_ptr<Model> model = NULL;  // Declare here to ensure presence in scope

        if (w->modelWanted_ == "Karpenka12") {
            std::shared_ptr<Karpenka12> karpenka12(new Karpenka12);
            karpenka12->x_ = vmath::sub<double>(lc.mjd_, lc.mjdMin_);
            karpenka12->y_ = vmath::div<double>(lc.flux_, lc.normalization_);
            karpenka12->sigma_ = std::vector<double>(lc.flux_.size(), 0.001);
            model = std::dynamic_pointer_cast<Model>(karpenka12);
        } else if (w->modelWanted_ == "Kessler10") {
            std::shared_ptr<Kessler10> kessler10(new Kessler10);
            kessler10->x_ = vmath::sub<double>(lc.mjd_, lc.mjdMin_);
            kessler10->y_ = vmath::div<double>(lc.flux_, lc.normalization_);
            kessler10->sigma_ = std::vector<double>(lc.flux_.size(), 0.001);
            model = std::dynamic_pointer_cast<Model>(kessler10);
        } else if (w->modelWanted_ == "Bazin09") {
            std::shared_ptr<Bazin09> bazin09(new Bazin09);
            bazin09->x_ = vmath::sub<double>(lc.mjd_, lc.mjdMin_);
            bazin09->y_ = vmath::div<double>(lc.flux_, lc.normalization_);
            bazin09->sigma_ = std::vector<double>(lc.flux_.size(), 0.001);
            model = std::dynamic_pointer_cast<Model>(bazin09);
        } else if (w->modelWanted_ == "Karpenka12Afterglow") {
            std::shared_ptr<Karpenka12Afterglow> karpenka12afterglow(new Karpenka12Afterglow);
            karpenka12afterglow->x_ = vmath::sub<double>(lc.mjd_, lc.mjdMin_);
            karpenka12afterglow->y_ = vmath::div<double>(lc.flux_, lc.normalization_);
            karpenka12afterglow->sigma_ = std::vector<double>(lc.flux_.size(), 0.001);
            model = std::dynamic_pointer_cast<Model>(karpenka12afterglow);
        } else if (w->modelWanted_ == "Firth17Complex") {
            std::shared_ptr<Firth17Complex> firth17complex(new Firth17Complex);
            firth17complex->x_ = vmath::sub<double>(lc.mjd_, lc.mjdMin_);
            firth17complex->y_ = vmath::div<double>(lc.flux_, lc.normalization_);
            firth17complex->sigma_ = std::vector<double>(lc.flux_.size(), 0.001);
            model = std::dynamic_pointer_cast<Model>(firth17complex);
        } else {
            std::cout << "Either blank or didn't recognise " << w->modelWanted_ << ". Defaulting to Bazin09" << std::endl;
            std::shared_ptr<Bazin09> bazin09(new Bazin09);
            bazin09->x_ = vmath::sub<double>(lc.mjd_, lc.mjdMin_);
            bazin09->y_ = vmath::div<double>(lc.flux_, lc.normalization_);
            bazin09->sigma_ = std::vector<double>(lc.flux_.size(), 0.001);
            model = std::dynamic_pointer_cast<Model>(bazin09);

        }

        // Initialise solver
        MNest solver(model);
        solver.xRecon_ = vmath::range<double>(-15, lc.mjdMax_ - lc.mjdMin_ + 20, 1);
        solver.chainPath_ = "chains/" + sn.second.name_ + "/phase";
        std::cout << sn.second.name_  << std::endl;

        // Perform fitting
        solver.analyse();

        size_t indexMax = std::distance(solver.bestFit_.begin(),
                                        max_element(solver.bestFit_.begin(),
                                                    solver.bestFit_.end()));
        double mjdZeroPhase = solver.xRecon_[indexMax] + lc.mjdMin_;

        for (auto &spec : sn.second.spec_) {
            phaseFile << "spectra/" << utils::split(spec.second.file_, '/').back();
            phaseFile << " " << (spec.second.mjd_ - mjdZeroPhase) / (1.0 + sn.second.zRaw_) << "\n";
        }

        phaseFile.close(); // Close output file

        sn.second.saveSpec(mjdZeroPhase);
    }
}


int main (int argc, char* argv[]) {
    std::vector<std::string> options;
    std::shared_ptr<Workspace> w(new Workspace());

    utils::getArgv(argc, argv, options);

    // Read in filters and find the ID of the filter used to determine the phase
    w->filterPath_ = "data/filters";
    w->filters_ = std::shared_ptr<Filters>(new Filters(w->filterPath_));
    w->cosmology_ = std::shared_ptr<Cosmology>(new Cosmology(0));

    applyOptions(options, w);
    fillUnassigned(w);

    // run SpecPhase pipeline
    std::cout << "Scanning /recon.\n" << std::endl;
    scanRecon(w);

    std::cout << "Making Synthetic LC.\n" << std::endl;
    makeSyntheticLC(w);

    std::cout << "Running fitPhase\n" << std::endl;
    fitPhase(w);

    return 0;
}
