#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include "core/utils.hpp"
#include "core/Filters.hpp"
#include "vmath/loadtxt.hpp"
#include "vmath/convert.hpp"

using namespace std;
using namespace vmath;


class Workspace {
public:
    // Filter related variables
    string filter_;
    string filterPath_;
    shared_ptr<Filters> filters_;
    short FLTID_;

    // Spectra files related variable
    vector<string> SNName_;
    vector<string> spectra_;

    // Synthetic photometry
    vector<double> MJD_;
    vector<double> synthFlux_;
    vector<double> synthMJD_;

    // Temporary vectors for the spectrum wavelengths and bandpasses
    vector<double> wav_;
    vector<double> flux_;

    // Constructor
    Workspace();
};


Workspace::Workspace() {
    filterPath_ = "data/filters";
}


void help() {
    cout << "CoCo - SpecPhase: \n";
    cout << "Originally writen by Natasha Karpenka, ";
    cout << "currently maintained by Szymon Prajs (S.Prajs@soton.ac.uk) ";
    cout << "and Rob Firth.\n";
    cout << "\nUsage:\n";
    cout << "specphase filter_name\n";
    cout << endl;
}


/* Assign input options to workspace parameters */
void applyOptions(vector<string> &options, shared_ptr<Workspace> w) {
    if (options.size() < 1 || options[0] == "-h" || options[0] == "--help") {
        help();
        exit(0);

    } else if (options.size() == 1)  {
        // For any other extension just assign the file as the only LC
        w->filter_ = options[0];

    } else {
        cout << "Options are not currently implemented\n";
        cout << "Program will continue executing" << endl;
    }
}

/* Find all mangled spectra in the Recon folder and split by SN name */
void readRecon(shared_ptr<Workspace> w) {
    w->spectra_.clear();
    dirlist("recon", w->spectra_);

    string extension = "";
    for (auto it = w->spectra_.begin(); it != w->spectra_.end();) {
        extension = split(*it, '.')[1];
        if (extension != "spec") {
            it = w->spectra_.erase(it);
        } else {
            ++it;
        }
    }
    sort(w->spectra_.begin(), w->spectra_.end());

    w->SNName_.clear();
    w->MJD_.clear();

    string base = "";
    vector<string> tempSplit;
    for (auto file : w->spectra_) {
        base = baseName(file);
        split(base, '_', tempSplit);

        w->SNName_.push_back(tempSplit[0]);
        w->MJD_.push_back(atof(tempSplit[1].c_str()));
    }

    vector<string> SNList = w->SNName_;
    removeDuplicates<string>(SNList);
}


void syntheticFlux(shared_ptr<Workspace> w) {
    vector< vector<string> > tempList;

    // Loop though each spectrum and reset current when new SN found
    string currentSN = "";
    for (size_t i = 0; i < w->SNName_.size(); ++i) {
        if (currentSN != w->SNName_[i]) {
            if (currentSN != "") {
                /*TODO - fitPhase() before clearing*/
            }
            currentSN = w->SNName_[i];
            w->synthFlux_.clear();
            w->synthMJD_.clear();
        }

        // Load spectra into temporary vectors
        w->synthMJD_.push_back(w->MJD_[i]);
        loadtxt<string>("recon/" + w->spectra_[i], 2, tempList);
        w->wav_ = castString<double>(tempList[0]);
        w->flux_ = castString<double>(tempList[1]);

        // rescale filter responses and find synthetic flux
        w->filters_->rescale(w->wav_);
        w->synthFlux_.push_back(w->filters_->flux(w->flux_, w->filter_));

        cout << w->synthFlux_.size() << " " << w->synthMJD_.back() << " " << w->synthFlux_.back() << endl;
    }

    // Need to run fitPhase for the last SN
    /*TODO - fitPhase() run again*/
}


int main (int argc, char* argv[]) {
    vector<string> options;
    shared_ptr<Workspace> w(new Workspace());

    getArgv(argc, argv, options);
    applyOptions(options, w);

    // Read in filters and find the ID of the filter used to determine the phase
    w->filters_ = shared_ptr<Filters>(new Filters(w->filterPath_));
    w->FLTID_ = w->filters_->filterID_[w->filter_];

    readRecon(w);
    syntheticFlux(w);

    return 0;
}
