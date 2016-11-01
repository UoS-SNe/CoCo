#ifndef COCO_VMATH_LOADTXT_HPP_
#define COCO_VMATH_LOADTXT_HPP_

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace vmath {  // NAMESPACE vmath

template <typename T>
void loadtxt(const string &fileName, int l, vector< vector<T> > &data) {
    data.resize(l);
    for (int i = 0; i < l; ++i) {
        data[i].clear();
    }

    ifstream file;
    file.open(fileName.c_str());
    T temp;
    string s;

    while (getline(file, s)) {
        istringstream iss(s);

        for (int i = 0; i < l; ++i) {
            iss >> temp;
            data[i].push_back(temp);
        }
    }

    file.close();
}


template <typename T>
vector< vector<T> > loadtxt(const string &fileName, int l) {
    vector< vector<T> > data;
    loadtxt<T>(fileName, l, data);
    return data;
}


template <typename T>
void loadtxt(const string &fileName, int l, vector<T> &data) {
    vector< vector<T> > dataTemp;
    loadtxt<T>(fileName, l, dataTemp);
    if (l == 1) {
        data = dataTemp[0];

    } else {
        cout << "Cannot read multiple columns to a 1D vector" << endl;
        exit(0);
    }
}


} //NAMESPACE vmath

#endif
