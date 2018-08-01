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
}



#endif //PROJECT_TEMPOGRAM_WRAPPER_H
