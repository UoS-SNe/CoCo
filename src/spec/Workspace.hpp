#ifndef COCO_SPEC_WORKSPACE_HPP_
#define COCO_SPEC_WORKSPACE_HPP_

#include <vector>
#include <string>
#include "../core/Filters.hpp"

using namespace std;


struct SpecPoint {
    double wav_;
    double flux_;
};


struct SN {
    // Input
    string specFile_;
    string lcFile_;
    string SNName_;
    double z_;

    // Loaded files
    vector<SpecPoint> spec_;
};


class Workspace {
public:
    // Constructor
    Workspace();

    // Filter data
    shared_ptr<Filters> filters_;

    // File names
    string SpecListFile_;
    string filterPath_;

    // Lists
    vector< vector<string> > infoList_;
    vector<string> specList_;
    vector<string> snNameList_;
    vector<double> zList_;
    vector<SN> SNe_;
    vector<string> filterList_;
};

#endif
