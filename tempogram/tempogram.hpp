//
// Created by egordm on 25-7-2018.
//

#ifndef TEMPOGRAM_TEMPO_ESTIMATION_COMPUTE_FOURIER_COEFFICIENTS_H
#define TEMPOGRAM_TEMPO_ESTIMATION_COMPUTE_FOURIER_COEFFICIENTS_H

#include <armadillo>
#include <sigpack.h>
#include <cmath>
#include "compute_fourier_coefficients.h"

using namespace arma;
using namespace sp;

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
    inline std::tuple<cx_mat, vec, vec>
    novelty_curve_to_tempogram_dft(vec &novelty_curve, vec &bpm, double feature_rate,
                                   int tempo_window, int hop_length) {
        auto win_length = static_cast<int>(round(tempo_window * feature_rate));
        win_length = win_length + (win_length % 2) - 1;

        auto window = sp::hann(static_cast<const uword>(win_length));
        auto half_window = (int) round(win_length / 2.0);

        vec pad(static_cast<const uword>(half_window));
        pad.zeros();

        novelty_curve = join_cols(pad, novelty_curve);
        novelty_curve = join_cols(novelty_curve, pad);

        auto ret = compute_fourier_coefficients(novelty_curve, window, win_length - hop_length, bpm / 60.,
                                                feature_rate);

        std::get<0>(ret) = std::get<0>(ret) / sqrt((double) win_length) / sum(window) * win_length;
        std::get<1>(ret) = std::get<1>(ret) * 60;
        std::get<2>(ret) = std::get<2>(ret) - std::get<2>(ret)(0);

        return ret;
    }

    inline std::tuple<cx_mat, vec, vec>
    novelty_curve_to_tempogram_dft(vec &novelty_curve, vec &bpm, double feature_rate, int tempo_window) {
        return novelty_curve_to_tempogram_dft(novelty_curve, bpm, feature_rate, tempo_window,
                                              (int) ceil(feature_rate / 5.));
    };
}

#endif //TEMPOGRAM_TEMPO_ESTIMATION_COMPUTE_FOURIER_COEFFICIENTS_H
