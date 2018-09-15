//
// Created by egordm on 26-7-2018.
//

#ifndef PROJECT_COMPUTE_FOURIER_COEFFICIENTS_H
#define PROJECT_COMPUTE_FOURIER_COEFFICIENTS_H

#include <armadillo>
#include <cmath>
#include "fourier_utils.h"


using namespace arma;

namespace tempogram {
    /**
     * Normalizes a feature sequence according to the l^p norm
     * If the norm falls below threshold for a feature vector, then the normalized feature vector is set to be the
     * unit vector.
     * @param feature
     * @param p
     * @param threshold
     * @return normalized feature
     */
    cx_mat normalize_feature(const cx_mat &feature, unsigned int p, double threshold);

    vec novelty_smoothed_subtraction(const vec &novelty_curve, int sr, int hop_length);
}


#endif //PROJECT_COMPUTE_FOURIER_COEFFICIENTS_H
