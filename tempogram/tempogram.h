//
// Created by egordm on 25-7-2018.
//

#ifndef TEMPOGRAM_TEMPO_ESTIMATION_COMPUTE_FOURIER_COEFFICIENTS_H
#define TEMPOGRAM_TEMPO_ESTIMATION_COMPUTE_FOURIER_COEFFICIENTS_H

#include <Eigen/Dense>

using namespace Eigen;

namespace tempogram {
    ArrayXXd compute_fourier_coefficients(const ArrayXd &s, const ArrayXd &window, int n_overlap, const ArrayXd &f,
                                      float sr = 1);

}

#endif //TEMPOGRAM_TEMPO_ESTIMATION_COMPUTE_FOURIER_COEFFICIENTS_H
