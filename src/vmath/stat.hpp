#ifndef COCO_VMATH_STAT_HPP_
#define COCO_VMATH_STAT_HPP_

#include <math.h>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

namespace vmath {


template<typename T>
T max(vector<T> &vec) {
    return *max_element(vec.begin(), vec.end());
}


template<typename T>
T min(vector<T> &vec) {
    return *min_element(vec.begin(), vec.end());
}

template <typename T>
T sum(vector<T> &vec) {
    return accumulate(vec.begin(), vec.end(), 0.0);
}


template <typename T>
T mean(vector<T> &vec) {
    return sum<T>(vec) / vec.size();
}


template <typename T>
T median(vector<T> vec) {
    sort(vec.begin(), vec.end());
    if (vec.size() % 2 == 0) {
        return (vec[vec.size() / 2 - 1] + vec[vec.size() / 2]) / 2;
    } else {
        return vec[vec.size() / 2];
    }
}


template <typename T>
T medianSigma(vector<T> vec) {
    sort(vec.begin(), vec.end());
    T med = median<T>(vec);

    T A = med - vec[round(0.16 * vec.size())];
    T B = vec[vec.size() - round(0.16 * vec.size())] - med;

    return std::min(A, B);
}


template<typename T>
T stdev(vector<T> &vec) {
    T res = 0.0;
    T m = mean<T>(vec);

    for_each(vec.begin(), vec.end(), [&](T d) {
        res += pow(d - m, 2);
    });

    return sqrt(res / (vec.size() - 1));
}


} //NAMESPACE vmath

#endif
