// CoCo - Supernova templates and simulations package
// Copyright (C) 2014, 2016, 2017  Szymon Prajs
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Contact author: S.Prajs@soton.ac.uk

#ifndef COCO_VMATH_LOADTXT_HPP_
#define COCO_VMATH_LOADTXT_HPP_

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


namespace vmath {  // NAMESPACE vmath

// Load text file with l columns
template <typename T>
void loadtxt(const std::string &fileName, int l, std::vector< std::vector<T> > &data) {
    // Prepare the output data vector
    data.resize(l);
    for (size_t i = 0; i < l; ++i) {
        data[i].clear();
    }

    // Open the text file into a buffer
    std::ifstream file;
    file.open(fileName.c_str());

    // Loop througheach line and pass into a stringstream
    T temp;
    std::string s;
    while (std::getline(file, s)) {
        if(!(s[0] == '#' || s.size() < (2*l - 1))) {
            std::istringstream iss(s);

            // loop though each column and save the output
            for (size_t i = 0; i < l; ++i) {
                iss >> temp;
                data[i].push_back(temp);
            }
        }
    }

    file.close();
}


template <typename T>
std::vector< std::vector<T> > loadtxt(const std::string &fileName, int l) {
    std::vector< std::vector<T> > data;
    loadtxt<T>(fileName, l, data);
    return data;
}


template <typename T>
void loadtxt(const std::string &fileName, int l, std::vector<T> &data) {
    std::vector< std::vector<T> > dataTemp;
    loadtxt<T>(fileName, l, dataTemp);
    if (l == 1) {
        data = dataTemp[0];

    } else {
        std::cout << "Cannot read multiple columns to a 1D std::vector" << std::endl;
        exit(0);
    }
}


} //NAMESPACE vmath

#endif
