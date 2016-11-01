#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include "core/utils.hpp"
#include "core/SN.hpp"
#include "vmath/loadtxt.hpp"
#include "vmath/algebra.hpp"
#include "lc/Workspace.hpp"
#include "lc/Model.hpp"
#include "lc/Solver.hpp"
#include "lc/MultiNest.hpp"

using namespace std;


void help() {
    cout << "TO_BE_NAMED: \n";
    cout << "Originally writen by Natasha Karpenka, ";
    cout << "currently maintained by Szymon Prajs (S.Prajs@soton.ac.uk) ";
    cout << "and Rob Firth.\n";
    cout << "\nUsage:\n";
    cout << "lcfit lightcurve_file.*   or  lightcurve_files_list.list";
    cout << endl;
}


/* Assign input options to workspace parameters */
void applyOptions(vector<string> &options, shared_ptr<Workspace> w) {
    if (options.size() < 1 || options[0] == "-h" || options[0] == "--help") {
        help();
        exit(0);
    }

    // First option is the LC file name or list of LC files
    w->LCListFile_ = options[0];
    if (options[0].substr(options[0].find_last_of(".") + 1) == "list") {
        vmath::loadtxt<string>(w->LCListFile_, 1, w->fileList_);

    } else {
        // For any other extension just assign the file as the only LC
        w->fileList_ = {w->LCListFile_};
    }


    // Go though each option and assign the correct properties
    vector<string> command;
    for (size_t i = 1; i < options.size(); ++i) {
        // Deal with flags by loading pairs of options into commands
        if (options[i] == "-f") {
            if (i+1 < options.size()) {
                command = {options[i], options[i+1]};
                i++;  // skip the next option as it's already assigned above

            } else {
                cout << options[i] << " is not a valid flag" << endl;
            }

        } else if (options[i] == "-h" || options[i] == "--help"){
            help();
            continue;

        } else {
            split(options[i], '=', command);
        }

        // Assign properties based on commands
        if (command.size() != 2) {
            cout << command[0] << " is not a valid command." << endl;
            continue;

        } else if (command[0] == "-f" ||
                   command[0] == "--filters" ||
                   command[0] == "--filter" ) {
            w->filterList_ = split(command[1], ',');

        } else {
            cout << command[0] << " is not a valid command." << endl;
        }
    }
}


/* Automatically fill in all unassigned properties with defaults */
void fillUnassigned(shared_ptr<Workspace> w) {
    // Do a sanity check for the LC files
    if (w->fileList_.size() == 0) {
        cout << "Something went seriously wrong.";
        cout << "Please consider report this bug on our project GitHub page";
        cout << endl;
        exit(0);
    }

	// Load the light curves
	for (auto lcfile : w->fileList_) {
		if (fileExists(lcfile)) {
			w->SNe_.push_back(lcfile);
		}
	}

    // Make a filter list
    if (w->filterList_.size() == 0) {
        // TODO - Look for filters in LC files
    }
}


/* Run the fitting routine for the SN with a given ID */
void fitSN(shared_ptr<Workspace> w, int ID) {
    w->SNID_ = ID;
    createDirectory(w->SNe_[w->SNID_].name_, "chains");
    MultiNest solver(w);

    // Open a text file for the recon file
    ofstream reconFile;
    reconFile.open("recon/" + w->SNe_[w->SNID_].name_ + ".dat");

    // Loop though every available filter
    for (size_t i = 0; i < w->SNe_[w->SNID_].filterList_.size(); ++i) {
        // Set up the truncated data vectors
        w->FLTID_ = i;
        w->data_.x_ = w->SNe_[w->SNID_].tList_[i];
        w->data_.y_ = w->SNe_[w->SNID_].fluxList_[i];
        w->data_.sigma_ = w->SNe_[w->SNID_].fluxErrList_[i];

        // Do the fitting (magic!)
        solver.fit();

        // Reset the units to original
        w->dataRecon_.x_ = add<double>(w->dataRecon_.x_, w->SNe_[w->SNID_].mjdMin_);
        w->dataRecon_.y_ = mult<double>(w->dataRecon_.y_, w->SNe_[w->SNID_].normalization_[w->FLTID_]);
        w->dataRecon_.sigma_ = mult<double>(w->dataRecon_.sigma_, w->SNe_[w->SNID_].normalization_[w->FLTID_]);

        // Write the data to reconFile text file buffor
        for (size_t j = 0; j < w->dataRecon_.x_.size(); ++j) {
            reconFile << w->dataRecon_.x_[j] << " " << w->dataRecon_.y_[j] << " " << w->dataRecon_.sigma_[j] << " " << w->SNe_[w->SNID_].filterList_[i] << "\n";
        }
    }
    reconFile.close();
}


/* Main program */
int main(int argc, char *argv[]) {
    vector<string> options;
    shared_ptr<Workspace> w(new Workspace());

    getArgv(argc, argv, options);
    applyOptions(options, w);
    fillUnassigned(w);

    // Create the chains and recon directories
    createDirectory("chains");
    createDirectory("recon");

    // TODO - This is a test
    fitSN(w, 0);

	return 0;
}
