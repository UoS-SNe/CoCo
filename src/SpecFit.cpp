#include <stdio.h>

#include <math.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>

#include "vmath/loadtxt.hpp"
#include "vmath/convert.hpp"
#include "vmath/algebra.hpp"

#include "core/utils.hpp"
#include "core/Cosmology.hpp"
#include "core/Filters.hpp"
#include "core/SN.hpp"


// Data structure for parameters that are passed between functions
struct Workspace {
    // User inputs
    std::string inputFileName_;
    std::vector<std::string> specFileList_;
    std::vector<std::string> snNameList_;
    std::vector<double> mjdList_;
    std::vector<double> zList_;

    // Hash table of SN light curves
    std::unordered_map<std::string, SN> sn_;

    // Other helper classes
    std::string filterPath_;
    std::shared_ptr<Cosmology> cosmology_;
    std::shared_ptr<Filters> filters_;
};


void help() {
    std::cout << "CoCo - SpecFit: \n";
    std::cout << "Originally developed by Natasha Karpenka, ";
    std::cout << "and reimplemented by Szymon Prajs (S.Prajs@soton.ac.uk).\n";
    std::cout << "Currently maintained by Szymon Prajs and Rob Firth.\n";
    std::cout << "\nUsage:\n";
    std::cout << "./specfit spectra_setup_file.list\n";
    std::cout << "or\n";
    std::cout << "./specfit spectrum_file.* SN_name MJD rsedshift\n\n";
    std::cout << " spectra_setup_file.list must have the following columns:\n";
    std::cout << "Spectrum_file_path SN_name MJD redshift\n";
    std::cout << std::endl;
}


/* Assign input options to workspace parameters */
void applyOptions(std::vector<std::string> &options, std::shared_ptr<Workspace> w) {
    if (options.size() < 1 || options[0] == "-h" || options[0] == "--help") {
        help();
        exit(0);
    }

    // First option is the LC file name or list of LC files
    double skipOptions;
    w->inputFileName_ = options[0];
    if (options[0].substr(options[0].find_last_of(".") + 1) == "list") {
        std::vector< std::vector<std::string> > infoList =
            vmath::loadtxt<std::string>(w->inputFileName_, 4);
        w->specFileList_ = infoList[0];
        w->snNameList_ = infoList[1];
        w->mjdList_ = vmath::castString<double>(infoList[2]);
        w->zList_ = vmath::castString<double>(infoList[3]);
        skipOptions = 1;

    } else if (options.size() >= 4) {
        w->specFileList_ = {options[0]};
        w->snNameList_ = {options[1]};
        w->mjdList_ = {atof(options[2].c_str())};
        w->zList_ = {atof(options[3].c_str())};
        skipOptions = 3;

    } else {
        std::cout << "You need to provide either a *.list or 4 parameters" << std::endl;
        exit(0);
    }


    // Go though each option and assign the correct properties
    std::vector<std::string> command;
    for (size_t i = skipOptions; i < options.size(); ++i) {
        // Deal with flags by loading pairs of options into commands
        if (options[i] == "-f") {
            if (i+1 < options.size()) {
                command = {options[i], options[i+1]};
                i++;  // skip the next option as it's already assigned above

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
        if (command.size() != 2) {
            std::cout << command[0] << " is not a valid command." << std::endl;
            continue;

        } else {
            std::cout << command[0] << " is not a valid command." << std::endl;
        }
    }
}


/* Automatically fill in all unassigned properties with defaults */
void fillUnassigned(std::shared_ptr<Workspace> w) {
    // Do a sanity check for the LC files
    if (w->specFileList_.size() == 0) {
        std::cout << "Something went seriously wrong.";
        std::cout << "Please consider report this bug on our project GitHub page";
        std::cout << std::endl;
        exit(0);
    }

	// Load data
    // TODO - Load specta and reconstructed LC's to a SN class object
}



int main(int argc, char *argv[]) {
    std::vector<std::string> options;
    std::shared_ptr<Workspace> w(new Workspace());
    w->cosmology_ = std::shared_ptr<Cosmology>(new Cosmology());

    utils::getArgv(argc, argv, options);
    applyOptions(options, w);
    fillUnassigned(w);

    // Load the filter responses
    w->filterPath_ = "data/filters";
    w->filters_ = std::shared_ptr<Filters>(new Filters(w->filterPath_));

    // TODO - Perform the fitting

    return 0;
}
