#ifndef COCO_SPEC_WORKSPACE_HPP_
#define COCO_SPEC_WORKSPACE_HPP_

#include <vector>
#include <string>
#include <iterator>
#include "../core/Filters.hpp"
#include "../core/LC.hpp"

using namespace std;


struct SN {
    // Input
    string specFile_;
    string lcFile_;
    string SNName_;
    double mjd_;
    double z_;

    // Loaded spectrum
    vector<double> wav_;
    vector<double> flux_;
    vector<double> fluxCorrected_;

    // Light curve slice
    LC lc_;
    vector<double> lcFlux_;
    vector<double> lcFluxError_;
    vector<double> lcCentralWavelength_;
    vector<string> lcFilters_;

    //Fit parameters
    vector<double> params_;
};


class Workspace {
public:
    // Constructor
    Workspace();

    // Active IDs
    short SNID_;

    // Filter data
    shared_ptr<Filters> filters_;

    // File names
    string SpecListFile_;
    string filterPath_;

    // Lists
    vector< vector<string> > infoList_;
    vector<string> specList_;
    vector<string> snNameList_;
    vector<double> mjdList_;
    vector<double> zList_;
    vector<SN> SNe_;
    vector<string> filterList_;

    // Functions
    void lcSlice();
};

#endif
