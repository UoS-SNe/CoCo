#ifndef COCO_VMATH_ALGEBRA_HPP_
#define COCO_VMATH_ALGEBRA_HPP_

#include <vector>
#include <math.h>

using namespace std;

namespace vmath {  // NAMESPACE vmath

template <typename T>
void add(const vector<T> &vec, T scalar, vector<T> &res) {
    res.resize(vec.size());

    for (int i = 0; i < vec.size(); ++i) {
        res[i] = vec[i] + scalar;
    }
}


template <typename T>
vector<T> add(vector<T> &vec, T scalar) {
    vector<T> res;
    add<T>(vec, scalar, res);
    return res;
}


template <typename T>
void add(const vector<T> &vec1, const vector<T> &vec2, vector<T> &res) {
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
vector<T> add(const vector<T> &vec1, const vector<T> &vec2) {
    vector<T> res;
    add<T>(vec1, vec2, res);
    return res;
}


template <typename T>
void sub(const vector<T> &vec, T scalar, vector<T> &res) {
    res.resize(vec.size());

    for (int i = 0; i < vec.size(); ++i) {
        res[i] = vec[i] - scalar;
    }
}


template <typename T>
vector<T> sub(vector<T> &vec, T scalar) {
    vector<T> res;
    sub<T>(vec, scalar, res);
    return res;
}


template <typename T>
void sub(const vector<T> &vec1, const vector<T> &vec2, vector<T> &res) {
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
vector<T> sub(const vector<T> &vec1, const vector<T> &vec2) {
    vector<T> res;
    sub<T>(vec1, vec2, res);
    return res;
}


template <typename T>
void mult(const vector<T> &vec, T scalar, vector<T> &res) {
    res.resize(vec.size());

    for (int i = 0; i < vec.size(); ++i) {
        res[i] = vec[i]*scalar;
    }
}


template <typename T>
vector<T> mult(const vector<T> &vec, T scalar) {
    vector<T> res;
    mult<T>(vec, scalar, res);
    return res;
}


template <typename T>
void mult(const vector<T> &vec1, const vector<T> &vec2, vector<T> &res) {
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
vector<T> mult(const vector<T> &vec1, const vector<T> &vec2) {
    vector<T> res;
    mult<T>(vec1, vec2, res);
    return res;
}


template <typename T>
void div(const vector<T> &vec, T scalar, vector<T> &res) {
    res.resize(vec.size());

    for (int i = 0; i < vec.size(); ++i) {
        res[i] = vec[i] / scalar;
    }
}


template <typename T>
vector<T> div(const vector<T> &vec, T scalar) {
    vector<T> res;
    div<T>(vec, scalar, res);
    return res;
}

template <typename T>
void div(T scalar, const vector<T> &vec, vector<T> &res) {
    res.resize(vec.size());

    for (int i = 0; i < vec.size(); ++i) {
        res[i] = scalar / vec[i];
    }
}


template <typename T>
vector<T> div(T scalar, const vector<T> &vec) {
    vector<T> res;
    div<T>(scalar, vec, res);
    return res;
}


template <typename T>
void div(const vector<T> &vec1, const vector<T> &vec2, vector<T> &res) {
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
vector<T> div(const vector<T> &vec1, const vector<T> &vec2) {
    vector<T> res;
    div<T>(vec1, vec2, res);
    return res;
}


template <typename T>
void power(const vector<T> &vec, double pwr, vector<T> &res) {
    res.resize(vec.size());

    for (int i = 0; i < vec.size(); ++i) {
        res[i] = pow(vec[i], pwr);
    }
}


template <typename T>
vector<T> power(vector<T> &vec, double pwr) {
    vector<T> res;
    power<T>(vec, pwr, res);
    return res;
}


} // NAMESPACE vmath

#endif
