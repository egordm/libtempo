//
// Created by egordm on 26-7-2018.
//

#define _USE_MATH_DEFINES
#include "compute_fourier_coefficients.h"
#include "math_utils.hpp"
#include <cmath>

using namespace tempogram::utils;
using namespace arma;

mat  tempogram::compute_fourier_coefficients(const vec &s, const vec &window, int n_overlap, const vec &f, double sr) {
    int win_length = (int)window.size();
    int hop_length = win_length - n_overlap;

    vec T = linspace<vec>(0, win_length - 1, static_cast<const uword>(win_length)) / sr;
    int win_num = math::fix((s.size() - n_overlap) / (win_length - n_overlap));
    mat x(static_cast<const uword>(win_num), f.size());

    vec twoPiT = 2 * M_PI * T;

    for(int f0 = 0; f0 < f.size(); ++f0) {
        vec twoPiFt = f[f0] * twoPiT;
        vec cosine = cos(twoPiFt);
        vec sine = sin(twoPiFt);

        for(int w = 0; w < win_num; ++w) {
            int start = w * hop_length;
            int stop = start + win_length;

            vec sig = s.subvec((const uword)start, (const uword)(stop - 1)) % window;
            auto co = sum(sig % cosine);
            auto si = sum(sig % sine);
            x((const uword)w, (const uword)f0) = (co + 1 * si);
        }
    }

    return x;
}
