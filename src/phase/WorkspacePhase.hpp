#ifndef COCO_PHASE_WORKSPACEPHASE_HPP_
#define COCO_PHASE_WORKSPACEPHASE_HPP_

#include <vector>
#include <string>
#include "../core/Filters.hpp"
#include "../lc/Model.hpp"

using namespace std;


class WorkspacePhase {
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
    vector< vector< vector<double> > > specDataList_;

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

    // Constructor
    WorkspacePhase();
};


#endif
