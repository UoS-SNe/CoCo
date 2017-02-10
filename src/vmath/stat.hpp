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

#ifndef COCO_VMATH_STAT_HPP_
#define COCO_VMATH_STAT_HPP_

#include <math.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

namespace vmath {


template<typename T>
T max(std::vector<T> &vec) {
    return *max_element(vec.begin(), vec.end());
}


template<typename T>
T min(std::vector<T> &vec) {
    return *std::min_element(vec.begin(), vec.end());
}


template<typename T>
T nearest(std::vector<T> &vec, T val) {
    auto i = std::min_element(vec.begin(), vec.end(), [=](T x, T y) {
        return fabs(x - val) < fabs(y - val);
    });

    return std::distance(vec.begin(), i);
}


template <typename T>
T sum(std::vector<T> &vec) {
    return std::accumulate(vec.begin(), vec.end(), 0.0);
}


template <typename T>
T mean(std::vector<T> &vec) {
    return sum<T>(vec) / vec.size();
}


template <typename T>
T median(std::vector<T> vec) {
    std::sort(vec.begin(), vec.end());
    if (vec.size() % 2 == 0) {
        return (vec[vec.size() / 2 - 1] + vec[vec.size() / 2]) / 2;
    } else {
        return vec[vec.size() / 2];
    }
}


template <typename T>
T medianSigma(std::vector<T> vec) {
    std::sort(vec.begin(), vec.end());
    T med = median<T>(vec);

    T A = med - vec[round(0.16 * vec.size())];
    T B = vec[vec.size() - round(0.16 * vec.size())] - med;

    return std::min(A, B);
}


template<typename T>
T stdev(std::vector<T> &vec) {
    T res = 0.0;
    T m = mean<T>(vec);

    std::for_each(vec.begin(), vec.end(), [&](T d) {
        res += pow(d - m, 2);
    });

    return sqrt(res / (vec.size() - 1));
}


} //NAMESPACE vmath

#endif
