#ifndef COCO_VMATH_CONVERT_HPP_
#define COCO_VMATH_CONVERT_HPP_

#include <math.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

namespace vmath {  // NAMESPACE vmath

template <typename T>
void castString(const vector<string> &s, vector<T> &res) {
    res.resize(s.size());
    T x;
    for (int i = 0; i < s.size(); ++i) {
        istringstream ss(s[i]);
        if (!(ss >> x))
            res[i] = -99;
        res[i] = x;
    }
}


template <typename T>
vector<T> castString(const vector<string> &s) {
    vector<T> res;
    castString<T>(s, res);
	return res;
}


template <typename T>
void transpose(const vector< vector<T> > &data, vector< vector<T> > &cols) {
    int x = data.size();
    int y = data[0].size();
    cols.resize(y);
    for (int i = 0; i < y; ++i) {
        cols[i].resize(x);
    }

    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            cols[j][i] = data[i][j];
        }
    }
}


template <typename T>
vector< vector<T> > transpose(const vector< vector<T> > &data) {
    vector< vector<T> > cols;
    transpose<T>(data, cols);
    return cols;
}


}  // NAMESPACE vmath


#endif
