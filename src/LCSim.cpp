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

#include "vmath/algebra.hpp"
#include "vmath/loadtxt.hpp"
#include "vmath/convert.hpp"

#include "core/Cosmology.hpp"
#include "core/utils.hpp"
#include "core/SN.hpp"
#include "core/Model.hpp"
#include "core/Solver.hpp"
#include "models/Karpenka12.hpp"
#include "solvers/MPFitter.hpp"


struct Workspace {
    std::vector<std::string> templateList_;
    std::vector<double> z_;
    std::vector<double> absMag_;
    std::vector<double> mjdPeak_;
    std::vector<double> Ebv_MW_;
    std::vector<double> Ebv_Host_;
    std::vector<double> R_v_;
    std::vector<std::string> simSetupList_;
    std::vector< std::vector<std::string> > simFilters_;
    std::vector< std::vector<double> > mjdSim_;

    std::unordered_map<std::string,SN> templateSNe_;
    std::shared_ptr<Filters> filters_;
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
    std::cout << "SN_name redshift abs_mag E(B-V)_MW E(B-V)_Host Rv_Host MJD_peak MJD+filters.list\n";
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
        w->Ebv_MW_ = vmath::castString<double>(inputData[3]);
        w->Ebv_Host_ = vmath::castString<double>(inputData[4]);
        w->R_v_ = vmath::castString<double>(inputData[5]);
        w->mjdPeak_ = vmath::castString<double>(inputData[6]);
        w->simSetupList_ = inputData[7];

    } else if (skip == 5) {
        w->templateList_ = {options[0]};
        w->z_ = {atof(options[1].c_str())};
        w->absMag_ = {atof(options[2].c_str())};
        w->Ebv_MW_ = {atof(options[3].c_str())};
        w->Ebv_Host_ = {atof(options[4].c_str())};
        w->R_v_ = {atof(options[5].c_str())};
        w->mjdPeak_ = {atof(options[6].c_str())};
        w->simSetupList_ = {options[7]};
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
    w->simFilters_.clear();
    for (size_t i = 0; i < w->templateList_.size(); ++i) {
        if (!utils::fileExists(w->templateList_[i])) {
            std::cout << "Setup file does not exist: " << w->templateList_[i] << std::endl;
            exit(0);
        }

        std::vector< std::vector<std::string> > temp =
          vmath::loadtxt<std::string>(w->templateList_[i], 2);
        w->mjdSim_.push_back(vmath::castString<double>(temp[0]));
        w->simFilters_.push_back(temp[1]);
    }

    std::vector<std::string> uniqueSN = w->templateList_;
    utils::removeDuplicates(uniqueSN);

    for (auto &snName : uniqueSN) {
        if (!utils::fileExists("recon/" + snName + ".phase")) {
            std::cout << "Template \"" << snName << "\" does not exists, exiting!";
            exit(0);
        }
        std::vector< std::vector<std::string> > phaseFile =
          vmath::loadtxt<std::string>("recon/" + snName + ".phase", 2);

        for (size_t i = 0; i < phaseFile[0].size(); ++i) {
            w->templateSNe_[snName].addSpec(phaseFile[0][i], atof(phaseFile[1][i].c_str()));
            w->templateSNe_[snName].z_ = 0.0;
        }
    }
}


// Split mjdSim_ into individual vectors for each filter
std::vector<double> mjdRange(std::string flt,
                             const std::vector<double> &mjd,
                             const std::vector<std::string> &filters) {
    std::vector<double> res;
    for (size_t i = 0; i < mjd.size(); ++i) {
        if (filters[i] == flt) {
            res.push_back(mjd[i]);
        }
    }

    return res;
}


// Simulate light curves
// This function needs to be thread safe. Workspace can only be used for read
// never write as this will cause race conditions. Each loop must have its
// own instance of the Cosmology class. Filters is a bit more of an issue.
// This will have to be addressed correctly later (TODO - Issue #23)
void simulate(std::shared_ptr<Workspace> w) {
    // Until the function is multi-threaded with openMP the cosmology class is safe here
    std::shared_ptr<Cosmology> cosmology(new Cosmology());
    double offset = 1;
    for (size_t i = 0; i < w->templateList_.size(); ++i) {
        // Make a working copy of the template spectra
        SN sn = w->templateSNe_[w->templateList_[i]];

        // Apply host galaxy reddening
        // TODO (Issue #21)- implement host galaxy reddening before redshifting
        sn.applyReddening(w->Ebv_Host_[i], w->R_v_[i]);

        // Move the spectra to new redshift
        sn.redshift(w->z_[i], cosmology);
        sn.moveMJD((1.0 + w->z_[i]), w->mjdPeak_[i]);

        // offset absolute magnitude
        offset = pow(10, 0.4 * (-17 - w->absMag_[i]));
        sn.scaleSpectra(offset);

        // Apply Milky Way extinction
        // TODO (Issue #21) - implement reddening at z=0
        sn.applyReddening(w->Ebv_MW_[i], 3.1);

        // synthesise LC for every unique filter
        std::vector<std::string> uniqueFilters = w->simFilters_[i];
        utils::removeDuplicates(uniqueFilters);
        sn.synthesiseLC(uniqueFilters, w->filters_);

        // Open the output file
        std::string outFileName = "sim/" + w->templateList_[i] + "_" +
          std::to_string(w->z_[i]) + "_" +
          std::to_string(w->absMag_[i]) + "_" +
          std::to_string(w->mjdPeak_[i]) + "_" +
          utils::baseName(w->simSetupList_[i]) + ".dat";
        std::ofstream outFile;
        outFile.open(outFileName);

        for (auto &lc : sn.lc_) {
            // Initialise model
            std::shared_ptr<Karpenka12> karpenka12(new Karpenka12);
            karpenka12->x_ = vmath::sub<double>(lc.second.mjd_, lc.second.mjdMin_);
            karpenka12->y_ = vmath::div<double>(lc.second.flux_, lc.second.normalization_);
            karpenka12->sigma_ = std::vector<double>(lc.second.flux_.size(), 1);
            std::shared_ptr<Model> model = std::dynamic_pointer_cast<Model>(karpenka12);

            // Initialise solver
            MPFitter solver(model);
            std::vector<double> xTemp = mjdRange(lc.second.filter_, w->mjdSim_[i], w->simFilters_[i]);
            solver.xRecon_ = vmath::sub<double>(xTemp, lc.second.mjdMin_);

            // Perform fitting
            solver.analyse();
            solver.xRecon_ = vmath::add<double>(solver.xRecon_, lc.second.mjdMin_);
            solver.bestFit_ = vmath::mult<double>(solver.bestFit_, lc.second.normalization_);

            for (size_t j = 0; j < solver.xRecon_.size(); ++j) {
                outFile << solver.xRecon_[i] << " ";
                outFile << "flux" << " ";
                outFile << lc.second.filter_ << "\n";
            }
        }
        outFile.close();
    }
}


int main(int argc, char *argv[]) {
    std::vector<std::string> options;
    std::shared_ptr<Workspace> w(new Workspace());
    w->filters_ = std::shared_ptr<Filters>(new Filters("data/filters"));

    utils::getArgv(argc, argv, options);
    applyOptions(options, w);
    fillUnassigned(w);

    // Perform the simulations
    utils::createDirectory("sim");
    simulate(w);

	return 0;
}
