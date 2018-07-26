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
    inline arma::mat novelty_curve_to_tempogram_dft(arma::vec &novelty_curve, arma::vec &bpm, double feature_rate,
                                                    int tempo_window, int hop_length) {
        auto win_length = static_cast<int>(round(tempo_window * feature_rate));
        win_length = win_length + (win_length % 2) - 1;

        auto window = sp::hann(static_cast<const uword>(win_length));
        auto half_window = (int) round(win_length / 2.0);

        arma::vec pad(static_cast<const uword>(half_window));
        pad.zeros();

        novelty_curve = join_cols(pad, novelty_curve);
        novelty_curve = join_cols(novelty_curve, pad);

        auto tempogram = compute_fourier_coefficients(novelty_curve, window, win_length - hop_length, bpm / 60.,
                                                      feature_rate);
        tempogram = tempogram / sqrt((double) win_length) / sum(window) * win_length;
//
        return tempogram;
    }
}

#endif //TEMPOGRAM_TEMPO_ESTIMATION_COMPUTE_FOURIER_COEFFICIENTS_H
