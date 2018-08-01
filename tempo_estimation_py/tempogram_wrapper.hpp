//
// Created by egordm on 28-7-2018.
//

#ifndef PROJECT_TEMPOGRAM_WRAPPER_H
#define PROJECT_TEMPOGRAM_WRAPPER_H

#include "pyarma.hpp"
#include <tempo_estimation.h>
#include <helper_functions.h>

namespace tempogram_wrapper {
    inline std::tuple<py::array, py::array, py::array>
    novelty_curve_to_tempogram_dft(pyarr_d novelty_curve_np, pyarr_d bpm_np, double feature_rate, int tempo_window,
                                   int hop_length) {
        arma::vec novelty_curve = py_to_arma_vec<double>(novelty_curve_np);
        arma::vec bpm = py_to_arma_vec<double>(bpm_np);
        auto ret = tempogram::novelty_curve_to_tempogram_dft(novelty_curve, bpm, feature_rate, tempo_window,
                                                             hop_length);

        return std::make_tuple(arma_to_py(std::get<0>(ret)),
                               arma_to_py(std::get<1>(ret)),
                               arma_to_py(std::get<2>(ret)));
    }

    inline py::array normalize_feature(pyarr_cd &feature_np, unsigned int p, double threshold) {
        auto feature = py_to_arma_mat<cx_double>(feature_np);
        auto ret = tempogram::normalize_feature(feature, p, threshold);
        return arma_to_py(ret);
    }

    inline std::tuple<py::array, int>
    audio_to_novelty_curve(pyarr_d signal_np, int sr, int window_length = -1, int hop_length = -1,
                           double compression_c = 1000, bool log_compression = true,
                           int resample_feature_rate = 200) {
        arma::vec signal = py_to_arma_vec<double>(signal_np);
        auto ret = tempogram::audio_to_novelty_curve(signal, sr, window_length, hop_length, compression_c,
                                                     log_compression,
                                                     resample_feature_rate);

        return std::make_tuple(arma_to_py(std::get<0>(ret)), std::get<1>(ret));
    };

    inline std::tuple<py::array, int, py::array, py::array, py::array>
    audio_to_novelty_curve_tempogram(pyarr_d signal_np, int sr, pyarr_d bpm_np, int tempo_window, int hop_length) {
        arma::vec signal = py_to_arma_vec<double>(signal_np);
        arma::vec bpm = py_to_arma_vec<double>(bpm_np);

        auto nov_cv = tempogram::audio_to_novelty_curve(signal, sr);
        auto ret = tempogram::novelty_curve_to_tempogram_dft(std::get<0>(nov_cv), bpm, std::get<1>(nov_cv), tempo_window,
                                                             hop_length);
        auto normalized_tempogram = tempogram::normalize_feature(std::get<0>(ret), 2, 0.0001);

        return std::make_tuple(
                arma_to_py(std::get<0>(nov_cv)), // novelty curve
                std::get<1>(nov_cv), // novelty curve feature rate
                arma_to_py(normalized_tempogram), // tempogram
                arma_to_py(std::get<1>(ret)), // tempogram frequencies
                arma_to_py(std::get<2>(ret))); // tempogram times
    };
}


#endif //PROJECT_TEMPOGRAM_WRAPPER_H
