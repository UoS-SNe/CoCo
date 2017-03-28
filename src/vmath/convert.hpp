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

#ifndef COCO_VMATH_CONVERT_HPP_
#define COCO_VMATH_CONVERT_HPP_

#include <math.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


namespace vmath {  // NAMESPACE vmath

template <typename T>
void castString(const std::vector<std::string> &s, std::vector<T> &res) {
    res.resize(s.size());
    T x;
    for (size_t i = 0; i < s.size(); ++i) {
        std::istringstream ss(s[i]);
        if (!(ss >> x))
            res[i] = -99;
        res[i] = x;
    }
}


template <typename T>
std::vector<T> castString(const std::vector<std::string> &s) {
    std::vector<T> res;
    castString<T>(s, res);
	return res;
}


template <typename T>
void transpose(const std::vector< std::vector<T> > &data, std::vector< std::vector<T> > &cols) {
    int x = data.size();
    int y = data[0].size();
    cols.resize(y);
    for (size_t i = 0; i < y; ++i) {
        cols[i].resize(x);
    }

    for (size_t i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            cols[j][i] = data[i][j];
        }
    }
}


template <typename T>
std::vector< std::vector<T> > transpose(const std::vector< std::vector<T> > &data) {
    std::vector< std::vector<T> > cols;
    transpose<T>(data, cols);
    return cols;
}


}  // NAMESPACE vmath


#endif
