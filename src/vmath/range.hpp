#ifndef COCO_VMATH_RANGE_HPP_
#define COCO_VMATH_RANGE_HPP_

#include <vector>
#include <math.h>

using namespace std;

namespace vmath {  // NAMESPACE vmath


template <typename T>
void range(T start, T end, T step, vector<T> &res) {
    res.clear();
    if (end < start) {
        step = -step;
    }
    for (T i = start; i < end; i+=step) {
        res.push_back(i);
    }
}


template <typename T>
vector<T> range(T start, T end, T step) {
    vector<T> res;
    range<T>(start, end, step, res) ;
    return res;
}


} // NAMESPACE vmath

#endif
