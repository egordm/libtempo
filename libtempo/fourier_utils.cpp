//
// Created by egordm on 15-9-2018.
//

#include "fourier_utils.h"

using namespace libtempo;

cx_mat fourier_utils::compute_fourier_coefficients(vec &t, const vec &s, const vec &window, int n_overlap, const vec &f,
                                                   double sr) {
    int win_length = (int) window.size();
    double win_length_half = win_length / 2.;
    int hop_length = win_length - n_overlap;

    vec twoPiT = (2 * M_PI / sr) * regspace<vec>(0, win_length - 1);
    int win_num = (int)utils::math::fix((s.size() - n_overlap) / (win_length - n_overlap));

    cx_mat x(static_cast<const uword>(win_num), f.size(), fill::zeros);
    t = linspace(win_length_half, win_length_half + (win_num - 1) * hop_length, (const uword) win_num) / sr;

#pragma omp parallel for
    for (int f0 = 0; f0 < f.size(); ++f0) {
        vec twoPiFt = f[f0] * twoPiT;
        vec cosine = cos(twoPiFt);
        vec sine = sin(twoPiFt);

        for (int w = 0; w < win_num; ++w) {
            int start = w * hop_length;
            int stop = start + win_length;

            vec sig = s.subvec((const uword) start, (const uword) (stop - 1)) % window;
            auto co = sum(sig % cosine);
            auto si = sum(sig % sine);

            x((const uword) w, (const uword) f0) = cx_double(co, si);
        }
    }

    return x.st();
}