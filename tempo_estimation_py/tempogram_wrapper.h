//
// Created by egordm on 28-7-2018.
//

#ifndef PROJECT_TEMPOGRAM_WRAPPER_H
#define PROJECT_TEMPOGRAM_WRAPPER_H

#include "pyarma.hpp"

namespace tempogram_wrapper {
    std::tuple<py::array, py::array, py::array>
    novelty_curve_to_tempogram_dft(pyarr_d novelty_curve_np, pyarr_d bpm_np, double feature_rate, int tempo_window,
                                   int hop_length);

    py::array normalize_feature(pyarr_cd &feature_np, unsigned int p, double threshold);
}



#endif //PROJECT_TEMPOGRAM_WRAPPER_H
