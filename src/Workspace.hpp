#ifndef COCO_WORKSPACE_HPP_
#define COCO_WORKSPACE_HPP_

#include <vector>
#include <string>
#include "Filters.hpp"
#include "SN.hpp"
#include "Model.hpp"

using namespace std;


struct Data {
    vector<double> x_;
    vector<double> y_;
    vector<double> sigma_;
};


class Workspace {
public:
    // Constructor
    Workspace();

    // File names
    string LCListFile_;
    string chainDir_;

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
