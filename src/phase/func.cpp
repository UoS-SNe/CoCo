#include "func.hpp"

using namespace std;


/* Find all mangled spectra in the Recon folder and split by SN name */
void readRecon(shared_ptr<WorkspacePhase> w) {
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
void syntheticFlux(shared_ptr<WorkspacePhase> w) {
    vector< vector<string> > tempList;

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
}


int resFunc(int m, int n, double *p, double *residual, double **dvec, void *vars) {
    class WorkspacePhase *w = (class WorkspacePhase *) vars;
    w->model_.params_.assign(p, p + n);

    for (size_t i = 0; i < w->synthMJD_.size(); ++i) {
        residual[i] = (w->synthFlux_[i] / w->maxFlux_) - w->model_(w->synthMJD_[i] - w->minMJD_);
    }

    return 0;
}


void fitPhase(shared_ptr<WorkspacePhase> w) {
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
    double magMax = -2.5 * log10(tempLC[indexMax]) - w->filters_->filters_[w->FLTID_].zp_;
    w->MJDPhaseZero_ = tempT[indexMax] + w->minMJD_;

    // Output file buffer
    ofstream phaseOutput;

    // Save the phases for each spectrum into a text file
    phaseOutput.open("recon/" + w->currentSN_ + ".phase");
    for (size_t i = 0; i < w->specList_.size(); ++i) {
        phaseOutput << w->specList_[i] << " " << w->MJDPhaseZero_ << " " << magMax << "\n";
    }

    phaseOutput.close();
}


void fit(shared_ptr<WorkspacePhase> w) {
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
