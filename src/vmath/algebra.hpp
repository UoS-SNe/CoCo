#ifndef COCO_VMATH_ALGEBRA_HPP_
#define COCO_VMATH_ALGEBRA_HPP_

#include <vector>
#include <math.h>

using namespace std;

namespace vmath {

template <typename T>
void add(const std::vector<T> &vec, T scalar, std::vector<T> &res) {
    res.resize(vec.size());

    for (int i = 0; i < vec.size(); ++i) {
        res[i] = vec[i] + scalar;
    }
}


template <typename T>
std::vector<T> add(std::vector<T> &vec, T scalar) {
    std::vector<T> res;
    add<T>(vec, scalar, res);
    return res;
}


template <typename T>
void add(const std::vector<T> &vec1, const std::vector<T> &vec2, std::vector<T> &res) {
    if (vec1.size() <= vec2.size()) {
        res.resize(vec1.size());
    } else {
        res.resize(vec2.size());
    }

    for (int i = 0; i < res.size(); ++i) {
        res[i] = vec1[i] + vec2[i];
    }
}


template <typename T>
std::vector<T> add(const std::vector<T> &vec1, const std::vector<T> &vec2) {
    std::vector<T> res;
    add<T>(vec1, vec2, res);
    return res;
}


template <typename T>
void sub(const std::vector<T> &vec, T scalar, std::vector<T> &res) {
    res.resize(vec.size());

    for (int i = 0; i < vec.size(); ++i) {
        res[i] = vec[i] - scalar;
    }
}


template <typename T>
std::vector<T> sub(std::vector<T> &vec, T scalar) {
    std::vector<T> res;
    sub<T>(vec, scalar, res);
    return res;
}


template <typename T>
void sub(const std::vector<T> &vec1, const std::vector<T> &vec2, std::vector<T> &res) {
    if (vec1.size() <= vec2.size()) {
        res.resize(vec1.size());
    } else {
        res.resize(vec2.size());
    }

    for (int i = 0; i < res.size(); ++i) {
        res[i] = vec1[i] - vec2[i];
    }
}


template <typename T>
std::vector<T> sub(const std::vector<T> &vec1, const std::vector<T> &vec2) {
    std::vector<T> res;
    sub<T>(vec1, vec2, res);
    return res;
}


template <typename T>
void mult(const std::vector<T> &vec, T scalar, std::vector<T> &res) {
    res.resize(vec.size());

    for (int i = 0; i < vec.size(); ++i) {
        res[i] = vec[i]*scalar;
    }
}


template <typename T>
std::vector<T> mult(const std::vector<T> &vec, T scalar) {
    std::vector<T> res;
    mult<T>(vec, scalar, res);
    return res;
}


template <typename T>
void mult(const std::vector<T> &vec1, const std::vector<T> &vec2, std::vector<T> &res) {
    if (vec1.size() <= vec2.size()) {
        res.resize(vec1.size());
    } else {
        res.resize(vec2.size());
    }

    for (int i = 0; i < res.size(); ++i) {
        res[i] = vec1[i]*vec2[i];
    }
}


template <typename T>
std::vector<T> mult(const std::vector<T> &vec1, const std::vector<T> &vec2) {
    std::vector<T> res;
    mult<T>(vec1, vec2, res);
    return res;
}


template <typename T>
void div(const std::vector<T> &vec, T scalar, std::vector<T> &res) {
    res.resize(vec.size());

    for (int i = 0; i < vec.size(); ++i) {
        res[i] = vec[i] / scalar;
    }
}


template <typename T>
std::vector<T> div(const std::vector<T> &vec, T scalar) {
    std::vector<T> res;
    div<T>(vec, scalar, res);
    return res;
}

template <typename T>
void div(T scalar, const std::vector<T> &vec, std::vector<T> &res) {
    res.resize(vec.size());

    for (int i = 0; i < vec.size(); ++i) {
        res[i] = scalar / vec[i];
    }
}


template <typename T>
std::vector<T> div(T scalar, const std::vector<T> &vec) {
    std::vector<T> res;
    div<T>(scalar, vec, res);
    return res;
}


template <typename T>
void div(const std::vector<T> &vec1, const std::vector<T> &vec2, std::vector<T> &res) {
    if (vec1.size() <= vec2.size()) {
        res.resize(vec1.size());
    } else {
        res.resize(vec2.size());
    }

    for (int i = 0; i < res.size(); ++i) {
        res[i] = vec1[i] / vec2[i];
    }
}


template <typename T>
std::vector<T> div(const std::vector<T> &vec1, const std::vector<T> &vec2) {
    std::vector<T> res;
    div<T>(vec1, vec2, res);
    return res;
}


template <typename T>
void power(const std::vector<T> &vec, double pwr, std::vector<T> &res) {
    res.resize(vec.size());

    for (int i = 0; i < vec.size(); ++i) {
        res[i] = pow(vec[i], pwr);
    }
}


template <typename T>
std::vector<T> power(std::vector<T> &vec, double pwr) {
    std::vector<T> res;
    power<T>(vec, pwr, res);
    return res;
}


} // NAMESPACE vmath

#endif
