//
// Created by egordm on 28-7-2018.
//

#include <sigpack.h>
#include "tempo_estimation.h"

using namespace sp;

std::tuple<cx_mat, vec, vec> tempogram::novelty_curve_to_tempogram_dft(vec &novelty_curve, vec &bpm, double feature_rate,
                                                                       int tempo_window)  {
    return novelty_curve_to_tempogram_dft(novelty_curve, bpm, feature_rate, tempo_window,
                                          (int) ceil(feature_rate / 5.));
}

std::tuple<cx_mat, vec, vec>
tempogram::novelty_curve_to_tempogram_dft(vec &novelty_curve, vec &bpm, double feature_rate, int tempo_window,
                                          int hop_length)  {
    auto win_length = static_cast<int>(round(tempo_window * feature_rate));
    win_length = win_length + (win_length % 2) - 1;

    auto window = sp::hann(static_cast<const uword>(win_length));
    auto half_window = (int) round(win_length / 2.0);

    vec pad(static_cast<const uword>(half_window), fill::zeros);

    novelty_curve = join_cols(pad, novelty_curve);
    novelty_curve = join_cols(novelty_curve, pad);

    auto ret = tempogram::compute_fourier_coefficients(novelty_curve, window, win_length - hop_length, bpm / 60.,
                                            feature_rate);

    std::get<0>(ret) = std::get<0>(ret) / sqrt((double) win_length) / sum(window) * win_length;
    std::get<1>(ret) = std::get<1>(ret) * 60;
    std::get<2>(ret) = std::get<2>(ret) - std::get<2>(ret)(0);

    return ret;
}