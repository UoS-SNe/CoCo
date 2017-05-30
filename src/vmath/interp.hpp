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

#ifndef COCO_VMATH_INTERP_HPP_
#define COCO_VMATH_INTERP_HPP_

#include <vector>
#include <math.h>


namespace vmath {  // NAMESPACE vmath

template <typename T>
T interp(T org_x, const std::vector<T> &temp_x, const std::vector<T> &temp_y, T out = 0) {
    T org_y = out;
    T m;
    T c;

    if (org_x < temp_x.front() || org_x > temp_x.back()) {
        return org_y;
    }

    T stepTry = temp_x[1] - temp_x[0];
    T indexTry = int((org_x - temp_x[0]) / stepTry);
    T sizeTry = int((temp_x.back() - temp_x.front()) / stepTry) + 1;

    if (sizeTry == temp_x.size()) {
        if (org_x == temp_x[indexTry]) {
            org_y = temp_y[indexTry];

        } else if (org_x > temp_x[indexTry] && org_x < temp_x[indexTry+1]) {
            m = (temp_y[indexTry+1] - temp_y[indexTry]) / (temp_x[indexTry+1] - temp_x[indexTry]);
            c = temp_y[indexTry] - m * temp_x[indexTry];
            org_y = m * org_x + c;

        } else {
            sizeTry--;
        }
    }

    /*
     *TODO - Choose a better search algorithm
     */
    if (sizeTry != temp_x.size()) {
        for (size_t i = 0 ; i < (temp_x.size() - 1) ; i++) {
            if (temp_x[i] <= org_x && temp_x[i+1] >= org_x) {
                m = (temp_y[i+1] - temp_y[i])/(temp_x[i+1] - temp_x[i]);
                c = temp_y[i] - m*temp_x[i];
                org_y = m*org_x + c;
                break;
            }
        }
    }

    return org_y;
}


template <typename T>
std::vector<T> interp(const std::vector<T> &org_x, const std::vector<T> &temp_x, const std::vector<T> &temp_y, T out = 0) {
    std::vector<T> org_y(org_x.size(), out);

    for (size_t i = 0; i < org_x.size(); ++i) {
        org_y[i] = interp<T>(org_x[i], temp_x, temp_y, out);
    }

    return org_y;
}

} // NAMESPACE vmath

#endif
