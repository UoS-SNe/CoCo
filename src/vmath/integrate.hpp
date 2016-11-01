#ifndef COCO_VMATH_INTEGRATE_HPP_
#define COCO_VMATH_INTEGRATE_HPP_

#include <math.h>
#include <vector>

using namespace std;

namespace vmath {  // NAMESPACE vmath

template <typename T>
T trapz(const vector <T> &vec, T width) {
    T res = 0;
    for (int i = 0; i < (vec.size() - 1); ++i) {
        res += (vec[i] * width) + (0.5 * (vec[i+1] - vec[i]) * width);
    }
    return res;
}

template <typename T>
T trapz(const vector <T> &vec, const vector <T> &wav) {
    T res = 0;
    double width_iterator = 0;
    for (int i = 0; i < (vec.size() - 1); ++i) {
	width_iterator = wav[i+1] - wav[i];
        res += (vec[i] * width_iterator) + (0.5 * (vec[i+1] - vec[i]) * width_iterator);
    }
    return res;
}


} //NAMESPACE vmath

#endif
