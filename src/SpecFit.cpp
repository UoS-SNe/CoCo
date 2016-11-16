#include <stdio.h>
#include <math.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include "core/utils.hpp"
#include "core/LC.hpp"
#include "vmath/loadtxt.hpp"
#include "vmath/convert.hpp"
#include "spec/WorkspaceSpec.hpp"
#include "spec/MultiNest.hpp"

using namespace std;
using namespace vmath;


void help() {
    cout << "SPECFIT: \n";
    cout << "Originally writen by Natasha Karpenka, ";
    cout << "currently maintained by Szymon Prajs (S.Prajs@soton.ac.uk) ";
    cout << "and Rob Firth.\n";
    cout << "\nUsage:\n";
    cout << "./specfit spectra_setup_file.list\n";
    cout << "or\n";
    cout << "./specfit spectrum_file.* SN_name MJD rsedshift\n\n";
    cout << " spectra_setup_file.list must have the following columns:\n";
    cout << "Spectrum_file_path SN_name MJD redshift\n";
    cout << endl;
}


/* Assign input options to workspace parameters */
void applyOptions(vector<string> &options, shared_ptr<WorkspaceSpec> w) {
    if (options.size() < 1 || options[0] == "-h" || options[0] == "--help") {
        help();
        exit(0);
    }

    // First option is the LC file name or list of LC files
    double skipOptions;
    w->SpecListFile_ = options[0];
    if (options[0].substr(options[0].find_last_of(".") + 1) == "list") {
        loadtxt<string>(w->SpecListFile_, 4, w->infoList_);
        w->specList_ = w->infoList_[0];
        w->snNameList_ = w->infoList_[1];
        w->mjdList_ = castString<double>(w->infoList_[2]);
        w->zList_ = castString<double>(w->infoList_[3]);
        skipOptions = 1;

    } else if (options.size() == 4) {
        w->specList_ = {options[0]};
        w->snNameList_ = {options[1]};
        w->mjdList_ = {atof(options[2].c_str())};
        w->zList_ = {atof(options[3].c_str())};
        skipOptions = 3;

    } else {
        cout << "You need to provide either a *.list or 4 paramters" << endl;
        exit(0);
    }


    // Go though each option and assign the correct properties
    vector<string> command;
    for (size_t i = skipOptions; i < options.size(); ++i) {
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
void fillUnassigned(shared_ptr<WorkspaceSpec> w) {
    // Do a sanity check for the LC files
    if (w->specList_.size() == 0) {
        cout << "Something went seriously wrong.";
        cout << "Please consider report this bug on our project GitHub page";
        cout << endl;
        exit(0);
    }

	// Load all data
    w->SNe_.resize(w->specList_.size());
    vector< vector<double> > specFile;
    for (size_t i = 0; i < w->specList_.size(); ++i) {
        if (fileExists(w->specList_[i]) ||
        fileExists("recon/" + w->snNameList_[i] + ".dat")) {
            // Load propertries
			w->SNe_[i].specFile_ = w->specList_[i];
            w->SNe_[i].lcFile_ = "recon/" + w->snNameList_[i] + ".dat";
            w->SNe_[i].SNName_ = w->snNameList_[i];
            w->SNe_[i].mjd_ = w->mjdList_[i];
            w->SNe_[i].z_ = w->zList_[i];

            // Load spectrum
            specFile = loadtxt<double>(w->SNe_[i].specFile_, 2);
            w->SNe_[i].wav_ = specFile[0];
            w->SNe_[i].flux_ = specFile[1];

            // Load light curve
            w->SNe_[i].lc_ = LC(w->SNe_[i].lcFile_);

        } else {
            w->SNe_.pop_back();
        }
	}

    // Make a filter list
    if (w->filterList_.size() == 0) {
        // TODO - Look for filters in LC files
    }
}


void fitSpec(shared_ptr<WorkspaceSpec> w, int ID) {
    w->SNID_ = ID;
    w->filters_->rescale(w->SNe_[w->SNID_].wav_);
    MultiNest solver(w);
    solver.fit();
}


int main(int argc, char *argv[]) {
    vector<string> options;
    shared_ptr<WorkspaceSpec> w(new WorkspaceSpec());

    getArgv(argc, argv, options);
    applyOptions(options, w);
    fillUnassigned(w);

    // Load the filter responses
    w->filters_ = shared_ptr<Filters>(new Filters(w->filterPath_));

    // Make light curve slices matching the spectrum
    w->lcSlice();

    // Do all sorts of magic
    for (size_t i = 0; i < w->SNe_.size(); ++i) {
        fitSpec(w, i);
    }

    return 0;
}
