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

#include "vmath/loadtxt.hpp"
#include "vmath/convert.hpp"

#include "core/utils.hpp"
#include "core/SN.hpp"


struct Workspace {
    std::vector<std::string> templateList_;
    std::vector<double> z_;
    std::vector<double> absMag_;
    std::vector<double> mjdPeak_;
    std::vector<std::string> simSetupList_;

    std::unordered_map<std::string,SN> rawSNe_;
};


void help() {
    std::cout << "CoCo - LCSim: \n";
    std::cout << "Developed and implemented by Szymon Prajs (S.Prajs@soton.ac.uk).\n";
    std::cout << "Currently maintained by Szymon Prajs and Rob Firth.\n";
    std::cout << "Originally developed by Natasha Karpenka\n";
    std::cout << "\nUsage:\n";
    std::cout << "lcsim *.list\n";
    std::cout << "./lcsim SN_name redshift abs_mag MJD_peak MJD+filters.list\n\n";
    std::cout << "or\n";
    std::cout << "*.list file must have the following columns:\n";
    std::cout << "SN_name redshift abs_mag MJD_peak MJD+filters.list\n";
    std::cout << std::endl;
}


// Assign input options to workspace parameters
void applyOptions(std::vector<std::string> &options, std::shared_ptr<Workspace> w) {
    if (options.size() < 1 || options[0] == "-h" || options[0] == "--help") {
        help();
        exit(0);
    }

    // Number of inputs to skip depending on input type
    int skip = 1;

    // If the first parameter is a list then break it down into vectors
    if (options.size() >= 5) {
        if (options[0].substr(options[0].find_last_of(".") + 1) == "list") {
            skip = 1;
        } else {
            skip = 5;
        }
    } else if (options.size() >= 1 &&
             options[0].substr(options[0].find_last_of(".") + 1) == "list") {
        skip = 1;

    } else {
        skip = 0;
    }

    if (skip == 1) {
        std::vector< std::vector<std::string> > inputData = vmath::loadtxt<std::string>(options[0], 5);
        w->templateList_ = inputData[0];
        w->z_ = vmath::castString<double>(inputData[1]);
        w->absMag_ = vmath::castString<double>(inputData[2]);
        w->mjdPeak_ = vmath::castString<double>(inputData[3]);
        w->simSetupList_ = inputData[4];

    } else if (skip == 5) {
        w->templateList_ = {options[0]};
        w->z_ = {atof(options[1].c_str())};
        w->absMag_ = {atof(options[2].c_str())};
        w->mjdPeak_ = {atof(options[3].c_str())};
        w->simSetupList_ = {options[4]};
    }

    // Go though each option and assign the correct properties
    std::vector<std::string> command;
    for (size_t i = skip; i < options.size(); ++i) {
        // Deal with flags by loading pairs of options into commands
        if (options[i] == "-m") {
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

        } else if (command[0] == "-m" || command[0] == "--model" ) {
            // w->model_ = command[1];

        } else {
            std::cout << command[0] << " is not a valid command." << std::endl;
        }
    }
}


// Fill properties based on input parameters
void fillUnassigned(std::shared_ptr<Workspace> w) {
    std::vector<std::string> uniqueSN = w->templateList_;
    utils::removeDuplicates(uniqueSN);

    for (auto &snName : uniqueSN) {
        if (!utils::fileExists("recon/" + snName + ".phase")) {
            exit(0);
        }
        std::vector< std::vector<std::string> > phaseFile =
          vmath::loadtxt<std::string>("recon/" + snName + ".phase", 2);

        for (size_t i = 0; i < phaseFile[0].size(); ++i) {
            w->rawSNe_[snName].addSpec(phaseFile[0][i], atof(phaseFile[1][i].c_str()));
        }
    }
}


int main(int argc, char *argv[]) {
    std::vector<std::string> options;
    std::shared_ptr<Workspace> w(new Workspace());

    utils::getArgv(argc, argv, options);
    applyOptions(options, w);
    fillUnassigned(w);

	return 0;
}
