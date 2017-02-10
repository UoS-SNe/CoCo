// CoCo - Supernova templates and simulations package
// Copyright (C) 2017  Szymon Prajs
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

#ifndef COCO_VMATH_RANGE_HPP_
#define COCO_VMATH_RANGE_HPP_

#include <vector>
#include <math.h>


namespace vmath {  // NAMESPACE vmath


template <typename T>
void range(T start, T end, T step, std::vector<T> &res) {
    res.clear();
    if (end < start) {
        step = -step;
    }
    for (T i = start; i < end; i+=step) {
        res.push_back(i);
    }
}


template <typename T>
std::vector<T> range(T start, T end, T step) {
    std::vector<T> res;
    range<T>(start, end, step, res) ;
    return res;
}


} // NAMESPACE vmath

#endif
