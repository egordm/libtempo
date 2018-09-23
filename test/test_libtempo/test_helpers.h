//
// Created by egordm on 23-9-2018.
//

#ifndef PROJECT_TEST_HELPERS_H
#define PROJECT_TEST_HELPERS_H

#include <math_utils.hpp>

#define TEST_EPLSION 0.0000001

#define COMP_DBL(a, b) (abs(a - b) < TEST_EPLSION)

template <typename T>
inline bool compare_vec(const arma::Col<T> &a, const arma::Col<T> &b) {
    for (int i = 0; i < a.size(); ++i) {
        if(!COMP_DBL(a[i], b[i])) return false;
    }

    return true;
}

#endif //PROJECT_TEST_HELPERS_H
