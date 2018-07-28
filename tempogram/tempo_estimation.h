//
// Created by egordm on 28-7-2018.
//

#ifndef PROJECT_TEMPO_ESTIMATION_H
#define PROJECT_TEMPO_ESTIMATION_H

#include <armadillo>
#include "compute_fourier_coefficients.h"

using namespace arma;

namespace tempogram {
    /**
     * Computes a complex valued fourier tempogram for a given novelty curve
     * indicating note onset candidates in the form of peaks.
     * This implementation provides parameters for chosing fourier
     * coefficients in a frequency range corresponding to musically meaningful
     * tempo values in bpm.
     *
     * @param novelty_curve: a novelty curve indicating note onset positions
     * @param bpm: vector containing BPM values to compute
     * @param feature_rate: feature rate of the novelty curve (Hz). This needs to be set to allow for setting other parameters in seconds!
     * @param tempo_window: Analysis window length in seconds
     * @param hop_length: window hop length in frames (of novelty curve)
     * @return tempogram, time vector, bpm
     */
    std::tuple <cx_mat, vec, vec> novelty_curve_to_tempogram_dft(vec &novelty_curve, vec &bpm, double feature_rate,
                                   int tempo_window, int hop_length);

    std::tuple<cx_mat, vec, vec> novelty_curve_to_tempogram_dft(vec &novelty_curve, vec &bpm, double feature_rate,
                                                                int tempo_window);
};


#endif //PROJECT_TEMPO_ESTIMATION_H
