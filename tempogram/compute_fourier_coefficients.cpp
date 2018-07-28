//
// Created by egordm on 26-7-2018.
//

#define _USE_MATH_DEFINES

#include "compute_fourier_coefficients.h"
#include "math_utils.hpp"
#include <cmath>

using namespace tempogram;
using namespace arma;

std::tuple<cx_mat, vec, vec>
tempogram::compute_fourier_coefficients(const vec &s, const vec &window, int n_overlap, const vec &f, double sr) {
    int win_length = (int) window.size();
    double win_length_half = win_length / 2.;
    int hop_length = win_length - n_overlap;

    vec T = linspace<vec>(0, win_length - 1, static_cast<const uword>(win_length)) / sr;
    int win_num = utils::math::fix((s.size() - n_overlap) / (win_length - n_overlap));
    cx_mat x(static_cast<const uword>(win_num), f.size(), fill::zeros);
    vec t = linspace(win_length_half, win_length_half + (win_num - 1) * hop_length, (const uword)win_num) / sr;

    vec twoPiT = 2 * M_PI * T;

    int test = 0;

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

    return std::make_tuple(x.st(), f, t);
}

cx_mat tempogram::normalize_feature(const cx_mat &feature, unsigned int p, double threshold) {
    cx_mat ret(feature.n_rows, feature.n_cols, fill::zeros);

    // normalise the vectors according to the l^p norm
    cx_mat unit_vec(feature.n_rows, 1);
    unit_vec.ones();
    unit_vec = unit_vec / norm(unit_vec, p);

    for(int k = 0; k < feature.n_cols; ++k) {
        double n = norm(feature(span::all, (const uword)k), p);

        if(n < threshold) ret(span::all, (const uword)k) = unit_vec;
        else ret(span::all, (const uword)k) = feature(span::all, (const uword)k) / n;
    }

    return ret;
}
