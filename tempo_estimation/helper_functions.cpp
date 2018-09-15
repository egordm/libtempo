//
// Created by egordm on 26-7-2018.
//

#define _USE_MATH_DEFINES

#include <sigpack.h>
#include "helper_functions.h"
#include "math_utils.hpp"
#include "resample.h"

using namespace tempogram;
using namespace arma;
using namespace sp;

cx_mat tempogram::normalize_feature(const cx_mat &feature, unsigned int p, double threshold) {
    cx_mat ret(feature.n_rows, feature.n_cols, fill::zeros);

    // normalise the vectors according to the l^p norm
    cx_mat unit_vec(feature.n_rows, 1);
    unit_vec.ones();
    unit_vec = unit_vec / norm(unit_vec, p);

#pragma omp parallel for
    for (int k = 0; k < feature.n_cols; ++k) {
        double n = norm(feature(span::all, (const uword) k), p);

        if (n < threshold) ret(span::all, (const uword) k) = unit_vec;
        else ret(span::all, (const uword) k) = feature(span::all, (const uword) k) / n;
    }

    return ret;
}

vec tempogram::novelty_smoothed_subtraction(const vec &novelty_curve, int sr, int hop_length) {
    double smooth_len = 1.5;
    smooth_len = max(ceil(smooth_len * sr / (double) hop_length), 3.);
    rowvec smooth_filter = (utils::math::my_hanning((const uword) smooth_len)).t();
    rowvec novelty_curve_t = novelty_curve.t();
    mat local_average = conv2(novelty_curve_t, flipud(fliplr(smooth_filter / sum(smooth_filter))), "same");

    rowvec novelty_sub = (novelty_curve_t - local_average);
    novelty_sub = novelty_sub % (novelty_sub > 0);

    return novelty_sub.t();
}
