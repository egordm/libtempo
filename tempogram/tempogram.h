//
// Created by egordm on 25-7-2018.
//

#ifndef TEMPOGRAM_TEMPO_ESTIMATION_COMPUTE_FOURIER_COEFFICIENTS_H
#define TEMPOGRAM_TEMPO_ESTIMATION_COMPUTE_FOURIER_COEFFICIENTS_H

#include <armadillo>
#include <sigpack.h>

using namespace arma;
using namespace sp;

namespace tempogram {
    inline double novelty_curve_to_tempogram_dft(arma::vec &novelty_curve, arma::vec &bpm, double feature_rate,
                                          int tempo_window, int hop_length)  {
        std::cout << novelty_curve << std::endl;
        std::cout << bpm << std::endl;//
        return 10;
    }
}

#endif //TEMPOGRAM_TEMPO_ESTIMATION_COMPUTE_FOURIER_COEFFICIENTS_H
