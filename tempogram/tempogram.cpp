//
// Created by egordm on 25-7-2018.
//

#include <iostream>
#include "tempogram.h"
#include "math.h"

using namespace tempogram;

ArrayXXd tempogram::compute_fourier_coefficients(const ArrayXd &s, const ArrayXd &window, int n_overlap, const ArrayXd &f,
                                             float sr) {
    int window_length = static_cast<int>(window.rows());
    int hop_length = window_length - n_overlap;

    auto T = ArrayXd::LinSpaced(window_length, 0, window_length - 1);
    const int window_num = (int) math::fixf((float) (s.rows() - n_overlap) / (float) (window_length - n_overlap));
    //auto x = ArrayXXd::Zero(window_num, f.rows());
    ArrayXXd x(window_num, f.rows());

    auto twoPiT = 2.0 * M_PI * T;

    for(int f0 = 0; f0 < f.rows(); ++f0) {
        auto twoPiFt = f.coeff(f0) * twoPiT;
        auto cosine = twoPiFt.cos();
        auto sine = twoPiFt.sin();

        for(int w = 0; w < window_num; ++w) {
            int start = (w - 1) * hop_length;
            int stop = start + window_length;
            int count = stop - start;

            auto sig = s.block(start, 0, count, 1);
            double co = (sig * cosine).sum();
            double si = (sig * sine).sum();

            x(w, f0) = co + si;
        }
    }

    return x;
}
