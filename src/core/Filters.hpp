#ifndef COCO_CORE_FILTERS_HPP_
#define COCO_CORE_FILTERS_HPP_

#include <vector>
#include <map>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <math.h>
#include "utils.hpp"
#include "../vmath/loadtxt.hpp"
#include "../vmath/integrate.hpp"
#include "../vmath/interp.hpp"
#include "../vmath/range.hpp"
#include "../vmath/algebra.hpp"

using namespace std;
using namespace vmath;


struct FilterData {
    string name_;
    vector<double> inputWavelength_;
    vector<double> inputBandpass_;
    vector<double> restWavelength_;
    vector<double> wavelength_;
    vector<double> bandpass_;
    double area_;
    double zp_;
    double min_;
    double max_;
    double centralWavelength_;
};

class Filters {
private:
    // private data members
    string folderPath_;
    vector<string> fileList_;

    // private functions
    void readFolder();
    void loadFilter(int);

public:
    // public data members
    map<string,int> filterID_;
    map<int,string> filterName_;
    vector<FilterData> filters_;

    // constructor
    Filters(string);

    // public functions
    void rescale(double,double,double);   // start, end, step
    void rescale(const vector<double>&);  // wavelength range
    void rescale(double);                 // step, start and end set automatically

    // Flux for (wavelength, filterName)
    double flux(const vector<double>&, const string&);
};

#endif
