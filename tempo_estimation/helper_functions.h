//
// Created by egordm on 26-7-2018.
//

#ifndef PROJECT_COMPUTE_FOURIER_COEFFICIENTS_H
#define PROJECT_COMPUTE_FOURIER_COEFFICIENTS_H

#include <armadillo>
#include <cmath>


using namespace arma;

namespace tempogram {
    /**
     * @param s time domain signal
     * @param window vector containing window function
     * @param n_overlap overlap given in samples
     * @param f vector of frequencies values of fourier coefficients, in Hz
     * @param sr sampling rate of signal s in Hz
     * @return complex fourier coefficients, frequency vector, time vector
     */
    std::tuple<cx_mat, vec, vec> compute_fourier_coefficients(const vec &s, const vec &window, int n_overlap,
                                                              const vec &f, double sr);

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

    /**
     * Computes aspectrogram using a STFT (short-time fourier transform)
     * @param signal: wavefrom of audio signal
     * @param sr: sample rate
     * @param window
     * @param coefficient_range
     * @param n_fft: window length
     * @param hop_length
     * @return spectrogram, frequency vector, time vector
     */
    std::tuple<mat, float, vec, vec>
    stft(const vec &signal, int sr, const vec &window, std::tuple<int, int> coefficient_range, int n_fft = -1,
         int hop_length = -1);

    std::tuple<mat, float, vec, vec> stft(const vec &s, int sr, const vec &window, int n_fft = -1, int hop_length = -1);

    vec novelty_smoothed_subtraction(const vec &novelty_curve, int sr, int hop_length);


}


#endif //PROJECT_COMPUTE_FOURIER_COEFFICIENTS_H
