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

#ifndef COCO_VMATH_INTEGRATE_HPP_
#define COCO_VMATH_INTEGRATE_HPP_

#include <math.h>
#include <vector>


namespace vmath {  // NAMESPACE vmath

template <typename T>
T trapz(const std::vector <T> &vec, T width) {
    T res = 0;
    for (size_t i = 0; i < (vec.size() - 1); ++i) {
        res += (vec[i] * width) + (0.5 * (vec[i+1] - vec[i]) * width);
    }
    return res;
}

template <typename T>
T trapz(const std::vector <T> &vec, const std::vector <T> &wav) {
    T res = 0;
    double width_iterator = 0;
    for (size_t i = 0; i < (vec.size() - 1); ++i) {
	width_iterator = wav[i+1] - wav[i];
        res += (vec[i] * width_iterator) + (0.5 * (vec[i+1] - vec[i]) * width_iterator);
    }
    return res;
}


} //NAMESPACE vmath

#endif
