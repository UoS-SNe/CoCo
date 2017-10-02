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

#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <map>

#include "vmath/algebra.hpp"
#include "vmath/loadtxt.hpp"
#include "vmath/range.hpp"

#include "core/utils.hpp"
#include "core/SN.hpp"
#include "models/Bazin09.hpp"
#include "models/Kessler10.hpp"
#include "models/Karpenka12.hpp"
#include "models/Karpenka12Afterglow.hpp"
#include "models/Firth17Complex.hpp"
#include "solvers/MNest.hpp"


// Data structure for parameters that are passed between functions
struct Workspace {
    // User inputs
    std::string inputFileName_;
    std::vector<std::string> fileList_;
    std::vector<std::string> filterList_;
//    std::vector<std::string> modelWanted_; // List of models
    std::string modelWanted_;

    // Hash table of SN light curves
    std::unordered_map<std::string, SN> sn_;
};


// Display a help message if needed
void help() {
    std::cout << "CoCo - LCFit: \n";
    std::cout << "Originally writen by Natasha Karpenka, ";
    std::cout << "currently maintained by Szymon Prajs (S.Prajs@soton.ac.uk) ";
    std::cout << "and Rob Firth.\n";
    std::cout << "\nUsage:\n";
    std::cout << "lcfit lightcurve_file.*   or  lightcurve_files_list.list";
    std::cout << std::endl;
}


// Assign input options to workspace parameters
void applyOptions(std::vector<std::string> &options, std::shared_ptr<Workspace> w) {
    if (options.size() < 1 || options[0] == "-h" || options[0] == "--help") {
        help();
        exit(0);
    }

    // First option is the LC file name or list of LC files
    w->inputFileName_ = options[0];
    if (utils::fileExtention(w->inputFileName_) == "list") {
        vmath::loadtxt<std::string>(w->inputFileName_, 1, w->fileList_);

    } else {
        // For any other extension just assign the file as the only LC
        w->fileList_ = {w->inputFileName_};
    }


    // Iterate though each option and assign its properties
    std::vector<std::string> command;

    for (size_t i = 1; i < options.size(); ++i) {
        std::cout << i << std::endl;
        // Deal with flags by loading pairs of options into commands
        if (options[i] == "-f") {
            if (i+1 < options.size()) {
                command = {options[i], options[i+1]};
                i++;  // skip the next option once the previous is assigned

            } else {
                std::cout << options[i] << " is not a valid flag" << std::endl;
            }
        } else if (options[i] == "-m") {
            if (i+1 < options.size()) {
                command = {options[i], options[i+1]};
                i++;  // skip the next option once the previous is assigned

            } else {
                std::cout << options[i] << " is not a valid flag" << std::endl;
            }
        } else if (options[i] == "-h" || options[i] == "--help"){
            help();
            continue;

        } else {
            utils::split(options[i], '=', command);
        }

        // Assign properties based on commands
//        if (command.size() != 2) {
//            std::cout << command[0] << " is not a valid command." << std::endl;
//            continue;
//
//        } else if (command[0] == "-f" ||
//                   command[0] == "--filters" ||
//                   command[0] == "--filter" ) {
//            w->filterList_ = utils::split(command[1], ',');
        if (command[0] == "-f" ||
                   command[0] == "--filters" ||
                   command[0] == "--filter" ) {
                std::cout << "Filter is " << command[1] << std::endl;
                w->filterList_ = utils::split(command[1], ',');
            }
        if (command[0] == "-m" ||
               command[0] == "--mod" ||
               command[0] == "--model" ) {
//               std::cout << command[0] << std::endl;
               std::cout << "Model is " << command[1] << std::endl;
//              w->modelWanted_ = utils::split(command[1], ','); // List of models
                w->modelWanted_ = command[1]; // single model


            } else {
                std::cout << command[0] << " is not a valid command." << std::endl;
        }
    }
}


// Automatically fill in all unassigned properties with defaults
void fillUnassigned(std::shared_ptr<Workspace> w) {
    // Do a sanity check for the LC files
    if (w->fileList_.size() == 0) {
        std::cout << "Something went seriously wrong.";
        std::cout << "Please consider report this bug on our project GitHub page";
        std::cout << std::endl;
        exit(0);
    }

	// Load the light curves
	for (auto lcfile : w->fileList_) {
		if (utils::fileExists(lcfile)) {
			w->sn_[utils::baseName(lcfile)] = SN(lcfile);
		}
	}

    // TODO - user defined filter list is not yet implemented
    if (w->filterList_.size() == 0) {}
}


void fitLC(std::shared_ptr<Workspace> w) {
    // Loop though each SN
    for (auto sn : w->sn_) {
        utils::createDirectory(sn.second.name_, "chains");

        // Open output text files for light curve reconstructions
        std::ofstream reconLCFile;
        std::ofstream reconStatFile;
        reconLCFile.open("recon/" + sn.second.name_ + ".dat");
        reconStatFile.open("recon/" + sn.second.name_ + ".stat");

        // Loop though each filter
        for (auto lc : sn.second.lc_) {
            // Initialise the model
            std::cout << "Fitting " << lc.second.filter_ << std::endl;
            std::cout << "With model: " << w->modelWanted_ << std::endl;  // Check the passed args

            std::shared_ptr<Model> model = NULL;  // Declare here to ensure presence in scope

            if (w->modelWanted_ == "Karpenka12") {
                std::cout << "foo" << std::endl;
                std::shared_ptr<Karpenka12> karpenka12(new Karpenka12);
                karpenka12->x_ = vmath::sub<double>(lc.second.mjd_, lc.second.mjdMin_);
                karpenka12->y_ = vmath::div<double>(lc.second.flux_, lc.second.normalization_);
                karpenka12->sigma_ = vmath::div<double>(lc.second.fluxErr_, lc.second.normalization_);
                model = std::dynamic_pointer_cast<Model>(karpenka12);
            } else if (w->modelWanted_ == "Kessler10") {
                std::cout << "bar" << std::endl;
                std::shared_ptr<Kessler10> kessler10(new Kessler10);
                kessler10->x_ = vmath::sub<double>(lc.second.mjd_, lc.second.mjdMin_);
                kessler10->y_ = vmath::div<double>(lc.second.flux_, lc.second.normalization_);
                kessler10->sigma_ = vmath::div<double>(lc.second.fluxErr_, lc.second.normalization_);
                model = std::dynamic_pointer_cast<Model>(kessler10);
            } else if (w->modelWanted_ == "Bazin09") {
                std::cout << "spam" << std::endl;
                std::shared_ptr<Bazin09> bazin09(new Bazin09);
                bazin09->x_ = vmath::sub<double>(lc.second.mjd_, lc.second.mjdMin_);
                bazin09->y_ = vmath::div<double>(lc.second.flux_, lc.second.normalization_);
                bazin09->sigma_ = vmath::div<double>(lc.second.fluxErr_, lc.second.normalization_);
                model = std::dynamic_pointer_cast<Model>(bazin09);
            } else if (w->modelWanted_ == "Karpenka12Afterglow") {
                std::shared_ptr<Karpenka12Afterglow> karpenka12afterglow(new Karpenka12Afterglow);
                karpenka12afterglow->x_ = vmath::sub<double>(lc.second.mjd_, lc.second.mjdMin_);
                karpenka12afterglow->y_ = vmath::div<double>(lc.second.flux_, lc.second.normalization_);
                karpenka12afterglow->sigma_ = vmath::div<double>(lc.second.fluxErr_, lc.second.normalization_);
                model = std::dynamic_pointer_cast<Model>(karpenka12afterglow);
            } else if (w->modelWanted_ == "Firth17Complex") {
                std::shared_ptr<Firth17Complex> firth17complex(new Firth17Complex);
                firth17complex->x_ = vmath::sub<double>(lc.second.mjd_, lc.second.mjdMin_);
                firth17complex->y_ = vmath::div<double>(lc.second.flux_, lc.second.normalization_);
                firth17complex->sigma_ = vmath::div<double>(lc.second.fluxErr_, lc.second.normalization_);
                model = std::dynamic_pointer_cast<Model>(firth17complex);
            } else {
                std::cout << "eggs" << std::endl;
                std::cout << "Either blank or didn't recognise " << w->modelWanted_ << ". Defaulting to Bazin09" << std::endl;
                std::shared_ptr<Bazin09> bazin09(new Bazin09);
                bazin09->x_ = vmath::sub<double>(lc.second.mjd_, lc.second.mjdMin_);
                bazin09->y_ = vmath::div<double>(lc.second.flux_, lc.second.normalization_);
                bazin09->sigma_ = vmath::div<double>(lc.second.fluxErr_, lc.second.normalization_);
                model = std::dynamic_pointer_cast<Model>(bazin09);
            }

            // Initialise solver
            MNest solver(model);
            solver.xRecon_ = vmath::range<double>(-15, lc.second.mjdMax_ - lc.second.mjdMin_ + 20, 1);
            solver.chainPath_ = "chains/" + sn.second.name_ + "/" + lc.second.filter_;

            // Perform fitting
            solver.analyse();

            // Reset lc units to original
            solver.xRecon_ = vmath::add<double>(solver.xRecon_, lc.second.mjdMin_);
            solver.bestFit_ = vmath::mult<double>(solver.bestFit_, lc.second.normalization_);
            solver.mean_ = vmath::mult<double>(solver.mean_, lc.second.normalization_);
            solver.meanSigma_ = vmath::mult<double>(solver.meanSigma_, lc.second.normalization_);
            solver.median_ = vmath::mult<double>(solver.median_, lc.second.normalization_);
            solver.medianSigma_ = vmath::mult<double>(solver.medianSigma_, lc.second.normalization_);

            // Write results to files
            for (size_t j = 0; j < solver.xRecon_.size(); ++j) {
                reconLCFile << solver.xRecon_[j] << " " << solver.mean_[j] << " ";
                reconLCFile << solver.meanSigma_[j] << " " << lc.second.filter_ << "\n";

                reconStatFile << solver.xRecon_[j] << " " << solver.mean_[j] << " ";
                reconStatFile << solver.meanSigma_[j] << " " << solver.bestFit_[j] << " ";
                reconStatFile << solver.median_[j] << " " << solver.medianSigma_[j] << " ";
                reconStatFile << lc.second.filter_ << "\n";
            }
        }

        reconLCFile.close();
        reconStatFile.close();
    }
}


// Main program
int main(int argc, char *argv[]) {
    std::vector<std::string> options;
    std::shared_ptr<Workspace> w(new Workspace);

    utils::getArgv(argc, argv, options);

//    for (size_t i = 1; i < options.size(); ++i) {
//        std::cout << options[i] << ", " << std::endl;
//        }

    applyOptions(options, w);
    fillUnassigned(w);

    // Create the chains and recon directories
    utils::createDirectory("chains");
    utils::createDirectory("recon");

    // Perform light curve fitting
    fitLC(w);

	return 0;
}
