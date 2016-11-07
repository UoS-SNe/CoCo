#ifndef COCO_CORE_LC_H_
#define COCO_CORE_LC_H_

#include <memory>
#include <algorithm>
#include <vector>
#include <limits>
#include "utils.hpp"
#include "../vmath/loadtxt.hpp"
#include "../vmath/convert.hpp"
#include "../vmath/stat.hpp"
#include "../vmath/algebra.hpp"

using namespace std;
using namespace vmath;


class LC {
private:
	vector<double> completeMJD_;
    vector<double> completeFlux_;
    vector<double> completeFluxErr_;
    vector<string> completeFilter_;

public:
	string name_;
	double mjdMin_;
	double mjdMax_;

	vector<double> normalization_;
	vector<double> mjdMinList_;
	vector<double> mjdMaxList_;

    vector<double> mjd_;
    vector<double> t_;
    vector<double> flux_;
    vector<double> fluxErr_;
    vector<string> filter_;

	vector< vector<double> > mjdList_;
    vector< vector<double> > tList_;
    vector< vector<double> > fluxList_;
    vector< vector<double> > fluxErrList_;
    vector<string> filterList_;

    LC(string);
	void readData(string);
    void restoreCompleteLC();
    void setFilterList();
    void removeData(double, double);
    void removeData(string);
	void makeDataLists();
	void normalizeDataLists();
};

#endif
