//
// Created by egordm on 21-9-2018.
//

#ifndef PROJECT_WRAPPER_SIGNAL_PROCESSING_H
#define PROJECT_WRAPPER_SIGNAL_PROCESSING_H

#include <tuple>
#include <fourier_utils.h>
#include "wrapper_libtempo.hpp"

using namespace wrapper_libtempo;
using namespace libtempo;

namespace wrapper_signal_processing {
    inline std::tuple<MatrixWrapper<double>, MatrixWrapper<double>, MatrixWrapper<double>, float>
    stft(const MatrixWrapper<double> &signal, int sr, const MatrixWrapper<double> &window,
         const std::tuple<int, int> &coefficient_range, int n_fft = -1, int hop_length = -1) {
        float feature_rate = 0;
        vec t;
        vec f;
        auto stft = fourier_utils::stft<double>(feature_rate, t, f, signal.data, sr, window.data, coefficient_range, n_fft,
                                        hop_length);
        return std::make_tuple(MatrixWrapper<double>(stft), MatrixWrapper<double>(t), MatrixWrapper<double>(f),
                               feature_rate);
    }

    inline std::tuple<MatrixWrapper<double>, MatrixWrapper<double>, MatrixWrapper<double>, float>
    stft_noc(const MatrixWrapper<double> &signal, int sr, const MatrixWrapper<double> &window, int n_fft = -1,
         int hop_length = -1) {
        float feature_rate = 0;
        vec t;
        vec f;
        auto stft = fourier_utils::stft<double>(feature_rate, t, f, signal.data, sr, window.data, n_fft, hop_length);
        return std::make_tuple(MatrixWrapper<double>(stft), MatrixWrapper<double>(t), MatrixWrapper<double>(f),
                               feature_rate);
    }

    inline std::tuple<MatrixWrapper<cx_double>, MatrixWrapper<double>>
    compute_fourier_coefficients(const MatrixWrapper<double> &s, const MatrixWrapper<double> &window, int n_overlap,
                                 const MatrixWrapper<double> &f, double sr) {
        vec t;
        auto ret = fourier_utils::compute_fourier_coefficients(t, s.data, window.data, n_overlap, f.data, sr);
        return  std::make_tuple(MatrixWrapper<cx_double>(ret), MatrixWrapper<double>(t));
    }
}

#endif //PROJECT_WRAPPER_SIGNAL_PROCESSING_H
