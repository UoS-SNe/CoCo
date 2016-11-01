#include <stdio.h>
#include <math.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <gsl/gsl_bspline.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_errno.h>
#include "core/utils.hpp"
#include "vmath/loadtxt.hpp"

using namespace std;
using namespace vmath;


void help() {
    cout << "SPECFIT: \n";
    cout << "Originally writen by Natasha Karpenka, ";
    cout << "currently maintained by Szymon Prajs (S.Prajs@soton.ac.uk) ";
    cout << "and Rob Firth.\n";
    cout << "\nUsage:\n";
    cout << "specfit [rest TBD for now]";
    cout << endl;
}


int main(int argc, char *argv[]) {
    vector<string> options;

    getArgv(argc, argv, options);
    help();
    
    return 0;
}
