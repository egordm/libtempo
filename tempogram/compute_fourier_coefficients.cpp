//
// Created by egordm on 26-7-2018.
//

#define _USE_MATH_DEFINES
#include "compute_fourier_coefficients.h"
#include "math_utils.hpp"
#include <cmath>

using namespace tempogram::utils;

arma::mat
tempogram::compute_fourier_coefficients(const arma::vec &s, const arma::vec &window, int n_overlap, const arma::vec &f,
                                        double sr) {
    int win_length = (int)window.size();
    int hop_length = win_length - n_overlap;

    auto T = arma::linspace<arma::vec>(0, win_length - 1, static_cast<const arma::uword>(win_length)) / sr;
    int win_num = math::fix((s.size() - n_overlap) / (win_length - n_overlap));
    arma::mat x(static_cast<const arma::uword>(win_num), f.size());

    auto twoPiT = 2 * M_PI * T;

    for(int f0 = 0; f0 < f.size(); ++f0) {
        auto twoPiFt = f[f0] * twoPiT;
        auto cosine = cos(twoPiFt);
        auto sine = sin(twoPiFt);

        for(int w = 0; w < win_num; ++w) {
            int start = w * hop_length;
            int stop = start + win_length;

            auto sig = s.subvec((const arma::uword)start, (const arma::uword)(stop - 1)) * window;
            auto co = sum(sig * cosine);
            auto si = sum(sig * sine);
            x[w, f0] = (co + 1 * si);
        }
    }

    return x;
}
