// TODO - Clean unnecessary includes
#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>

#include "vmath/loadtxt.hpp"
#include "vmath/algebra.hpp"

#include "core/utils.hpp"
#include "core/LC.hpp"
#include "lc/WorkspaceLC.hpp"
#include "lc/Model.hpp"
#include "lc/MultiNest.hpp"

#include "core/SN.hpp"


// Data structure for parameters that are passed between functions
struct Workspace {
    // User inputs
    std::string inputFileName_;
    std::vector<std::string> fileList_;
    std::vector<std::string> filterList_;

    // Hash table of SN light curves
    std::unordered_map<std::string, SN> sn_;
};


// Display a help message if needed
void help() {
    std::cout << "TO_BE_NAMED: \n";
    std::cout << "Originally writen by Natasha Karpenka, ";
    std::cout << "currently maintained by Szymon Prajs (S.Prajs@soton.ac.uk) ";
    std::cout << "and Rob Firth.\n";
    std::cout << "\nUsage:\n";
    std::cout << "lcfit lightcurve_file.*   or  lightcurve_files_list.list";
    std::cout << std::endl;
}


// Assign input options to workspace parameters
void applyOptions(std::vector<std::string> &options, shared_ptr<Workspace> w) {
    if (options.size() < 1 || options[0] == "-h" || options[0] == "--help") {
        help();
        exit(0);
    }

    // First option is the LC file name or list of LC files
    w->inputFileName_ = options[0];
    if (fileExtention(w->inputFileName_) == "list") {
        vmath::loadtxt<std::string>(w->inputFileName_, 1, w->fileList_);

    } else {
        // For any other extension just assign the file as the only LC
        w->fileList_ = {w->inputFileName_};
    }


    // Iterate though each option and assign its properties
    std::vector<std::string> command;
    for (size_t i = 1; i < options.size(); ++i) {
        // Deal with flags by loading pairs of options into commands
        if (options[i] == "-f") {
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
            split(options[i], '=', command);
        }

        // Assign properties based on commands
        if (command.size() != 2) {
            std::cout << command[0] << " is not a valid command." << std::endl;
            continue;

        } else if (command[0] == "-f" ||
                   command[0] == "--filters" ||
                   command[0] == "--filter" ) {
            w->filterList_ = split(command[1], ',');

        } else {
            std::cout << command[0] << " is not a valid command." << std::endl;
        }
    }
}


// Automatically fill in all unassigned properties with defaults
void fillUnassigned(shared_ptr<Workspace> w) {
    // Do a sanity check for the LC files
    if (w->fileList_.size() == 0) {
        std::cout << "Something went seriously wrong.";
        std::cout << "Please consider report this bug on our project GitHub page";
        std::cout << std::endl;
        exit(0);
    }

	// Load the light curves
	for (auto lcfile : w->fileList_) {
		if (fileExists(lcfile)) {
			w->sn_[baseName(lcfile)] = SN(lcfile);
		}
	}

    // TODO - user defined filter list is not yet implemented
    if (w->filterList_.size() == 0) {}
}


// Run the fitting routine for the SN with a given ID
void fitSN(shared_ptr<WorkspaceLC> w, int ID) {
    w->SNID_ = ID;
    createDirectory(w->SNe_[w->SNID_].name_, "chains");
    MultiNest solver(w);

    // Open a text file for the recon file
    ofstream reconLCFile;
    ofstream reconStatFile;
    reconLCFile.open("recon/" + w->SNe_[w->SNID_].name_ + ".dat");
    reconStatFile.open("recon/" + w->SNe_[w->SNID_].name_ + ".stat");

    // Loop though every available filter
    for (size_t i = 0; i < w->SNe_[w->SNID_].filterList_.size(); ++i) {
        // Set up the truncated data std::vectors
        w->FLTID_ = i;
        w->data_.x_ = w->SNe_[w->SNID_].tList_[i];
        w->data_.y_ = w->SNe_[w->SNID_].fluxList_[i];
        w->data_.sigma_ = w->SNe_[w->SNID_].fluxErrList_[i];

        // Do the fitting (magic!)
        solver.fit();

        // Reset the units to original
        w->dataRecon_.x_ = add<double>(w->dataRecon_.x_, w->SNe_[w->SNID_].mjdMinList_[w->FLTID_]);
        w->dataRecon_.y_ = mult<double>(w->dataRecon_.y_, w->SNe_[w->SNID_].normalization_[w->FLTID_]);
        w->dataRecon_.sigma_ = mult<double>(w->dataRecon_.sigma_, w->SNe_[w->SNID_].normalization_[w->FLTID_]);
        w->dataRecon_.bestFit_ = mult<double>(w->dataRecon_.bestFit_, w->SNe_[w->SNID_].normalization_[w->FLTID_]);
        w->dataRecon_.median_ = mult<double>(w->dataRecon_.median_, w->SNe_[w->SNID_].normalization_[w->FLTID_]);
        w->dataRecon_.medianSigma_ = mult<double>(w->dataRecon_.medianSigma_, w->SNe_[w->SNID_].normalization_[w->FLTID_]);

        // Write the data to reconLCFile text file buffor
        for (size_t j = 0; j < w->dataRecon_.x_.size(); ++j) {
            reconLCFile << w->dataRecon_.x_[j] << " " << w->dataRecon_.y_[j] << " ";
            reconLCFile << w->dataRecon_.sigma_[j] << " " << w->SNe_[w->SNID_].filterList_[i] << "\n";

            reconStatFile << w->dataRecon_.x_[j] << " " << w->dataRecon_.y_[j] << " ";
            reconStatFile << w->dataRecon_.sigma_[j] << " " << w->dataRecon_.bestFit_[j] << " ";
            reconStatFile << w->dataRecon_.median_[j] << " " << w->dataRecon_.medianSigma_[j] << " ";
            reconStatFile << w->SNe_[w->SNID_].filterList_[i] << "\n";

        }
    }

    reconLCFile.close();
    reconStatFile.close();
}


// Main program
int main(int argc, char *argv[]) {
    std::vector<std::string> options;
    shared_ptr<WorkspaceLC> w(new WorkspaceLC());
    shared_ptr<Workspace> w2(new Workspace);

    getArgv(argc, argv, options);
    applyOptions(options, w2);
    fillUnassigned(w2);

    // Create the chains and recon directories
    createDirectory("chains");
    createDirectory("recon");

    for (size_t i = 0; i < w->SNe_.size(); ++i) {
        fitSN(w, i);
    }

	return 0;
}
