#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include "core/utils.hpp"
#include "core/Filters.hpp"
#include "core/mpfit.h"
#include "lc/Model.hpp"
#include "vmath/loadtxt.hpp"
#include "vmath/convert.hpp"
#include "vmath/stat.hpp"

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
    vector<string> specList_;

    // Synthetic photometry
    vector<double> MJD_;
    vector<double> synthFlux_;
    vector<double> synthMJD_;
    string currentSN_;
    double minMJD_;
    double maxFlux_;
    double MJDPhaseZero_;

    // Temporary vectors for the spectrum wavelengths and bandpasses
    vector<double> wav_;
    vector<double> flux_;

    // Model class
    Model model_;

    // Output file buffer
    ofstream phaseOutput_;

    // Constructor
    Workspace();
};


Workspace::Workspace() {
    filterPath_ = "data/filters";
}


// Need to predeclare some functions so it can be used before being defined
void fitPhase(shared_ptr<Workspace>);
void fit(shared_ptr<Workspace>);

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

/**/
void syntheticFlux(shared_ptr<Workspace> w) {
    vector< vector<string> > tempList;
    w->phaseOutput_.open("phase.list");

    // Loop though each spectrum and reset current when new SN found
    w->currentSN_ = "";
    for (size_t i = 0; i < w->SNName_.size(); ++i) {
        if (w->currentSN_ != w->SNName_[i]) {
            if (w->currentSN_ != "") {
                fitPhase(w);
            }
            w->currentSN_ = w->SNName_[i];
            w->synthFlux_.clear();
            w->synthMJD_.clear();
            w->specList_.clear();
        }

        // Load spectra into temporary vectors
        w->specList_.push_back(w->spectra_[i]);
        w->synthMJD_.push_back(w->MJD_[i]);
        loadtxt<string>("recon/" + w->spectra_[i], 2, tempList);
        w->wav_ = castString<double>(tempList[0]);
        w->flux_ = castString<double>(tempList[1]);

        // rescale filter responses and find synthetic flux
        w->filters_->rescale(w->wav_);
        w->synthFlux_.push_back(w->filters_->flux(w->flux_, w->filter_));
    }

    // Need to run fitPhase for the last SN
    fitPhase(w);

    w->phaseOutput_.close();
}


int resFunc(int m, int n, double *p, double *residual, double **dvec, void *vars) {
    class Workspace *w = (class Workspace *) vars;
    w->model_.params_.assign(p, p + n);

    for (size_t i = 0; i < w->synthMJD_.size(); ++i) {
        residual[i] = (w->synthFlux_[i] / w->maxFlux_) - w->model_(w->synthMJD_[i] - w->minMJD_);
    }

    return 0;
}


void fitPhase(shared_ptr<Workspace> w) {
    // DEBUG - Test data set
    w->synthMJD_ = {53452.7,53468.7,53477.7,53489.7,53500.7,
                    53522.7,53527.7,53539.7,53550.7,53578.7};
    w->synthFlux_ = {9.43604e-17,4.39616e-16,5.8541e-16,1.05009e-15,1.3557e-15,
                   2.90792e-15,1.80171e-16,5.18103e-17,1.51663e-18,5.28895e-19};

    // Set up fit parameters
    w->model_.params_ = {1.0, 0.1, 1.0, 1.0, 10.0, 10.0, 10.0, 10.0};
    w->minMJD_ = min<double>(w->synthMJD_);
    w->maxFlux_ = max<double>(w->synthFlux_);

    // Do model fitting
    fit(w);

    // Create a temporary time vector normalised to minMJD
    vector<double> tempT = sub<double>(w->synthMJD_, w->minMJD_);
    sort(tempT.begin(), tempT.end());
    tempT = range<double>(tempT.front() - 20, tempT.back() + 20, 1);

    // Find the peak of the synthetic light curve
    vector<double> tempLC = w->model_(tempT);
    size_t indexMax = distance(tempLC.begin(), max_element(tempLC.begin(), tempLC.end()));
    w->MJDPhaseZero_ = tempT[indexMax] + w->minMJD_;

    // Save the phases for each spectrum into a text file
    for (size_t i = 0; i < w->specList_.size(); ++i) {
        w->phaseOutput_ << w->specList_[i] << " " <<  w->currentSN_ << " " << w->MJDPhaseZero_ << " " << w->synthMJD_[i] - w->MJDPhaseZero_ << "\n";
    }
}


void fit(shared_ptr<Workspace> w) {
    // Fit parameters
    vector<double> par = w->model_.params_;

    // MPFIT setup params - TODO
    int status;
    mp_result result;
    mp_config config;
    mp_par pars[par.size()];
    memset(&config, 0, sizeof(config));
    memset(&result, 0, sizeof(result));
    memset(&pars,0,sizeof(pars));
    vector<double> parErr(par.size());
    result.xerror = parErr.data();

    config.maxiter = 2000;
    status = mpfit(resFunc, w->synthFlux_.size(), par.size(), par.data(), pars, &config, (void*) w.get(), &result);
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
