#ifndef COCO_VMATH_STAT_HPP_
#define COCO_VMATH_STAT_HPP_

#include <math.h>
#include <vector>

using namespace std;

namespace vmath {  // NAMESPACE vmath

template<typename T>
T max(const vector<T> &vec) {
    return *max_element(vec.begin(), vec.end());
}



template <typename T>
T sum(vector<T> &vec) {
    return accumulate(vec.begin(), vec.end(), 0.0);
}


template <typename T>
T mean(const vector<T> &vec) {
    return sum<T>(vec) / vec.size();
}


template<typename T>
T min(const vector<T> &vec) {
    return *min_element(vec.begin(), vec.end());
}


template<typename T>
T standardDiv(const vector<T> &vec) {
    T res = 0.0;
    T m = mean<T>(vec);

    for_each(vec.begin(), vec.end(), [&](const T d) {
        res += pow(d - m, 2);
    });

    return sqrt(res / (vec.size() - 1));
}


} //NAMESPACE vmath

#endif
