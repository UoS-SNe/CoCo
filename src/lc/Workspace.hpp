#ifndef COCO_LC_WORKSPACE_HPP_
#define COCO_LC_WORKSPACE_HPP_

#include <vector>
#include <string>
#include "../core/SN.hpp"
#include "Model.hpp"

using namespace std;


struct Data {
    vector<double> x_;
    vector<double> y_;
    vector<double> sigma_;

    vector<double> bestFit_;
    vector<double> median_;
    vector<double> medianSigma_;
};


class Workspace {
public:
    // Constructor
    Workspace();

    // File names
    string LCListFile_;

    // Lists
    vector<string> fileList_;
    vector<string> filterList_;

    // List of SN objects
    vector<SN> SNe_;

    // Current supernova
    short SNID_;
    short FLTID_;

    // Light curve model - THIS WILL CHANGE IN THE FUTURE
    Model model_;
    Data data_;
    Data dataRecon_;
};

#endif
