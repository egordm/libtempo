//
// Created by egordm on 26-7-2018.
//

#ifndef PROJECT_COMPUTE_FOURIER_COEFFICIENTS_H
#define PROJECT_COMPUTE_FOURIER_COEFFICIENTS_H

#include <armadillo>

using namespace arma;

namespace tempogram {
    /**
     * @param s time domain signal
     * @param window vector containing window function
     * @param n_overlap overlap given in samples
     * @param f vector of frequencies values of fourier coefficients, in Hz
     * @param sr sampling rate of signal s in Hz
     * @return
     */
    std::tuple<cx_mat, vec, vec> compute_fourier_coefficients(const arma::vec &s, const arma::vec &window, int n_overlap, const arma::vec &f, double sr);

    /**
     * Normalizes a feature sequence according to the l^p norm
     * If the norm falls below threshold for a feature vector, then the normalized feature vector is set to be the
     * unit vector.
     * @param feature
     * @param p
     * @param threshold
     * @return
     */
    cx_mat normalize_feature(const cx_mat &feature, unsigned int p, double threshold);
}


#endif //PROJECT_COMPUTE_FOURIER_COEFFICIENTS_H
