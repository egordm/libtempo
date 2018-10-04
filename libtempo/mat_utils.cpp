//
// Created by egordm on 29-8-2018.
//

#include "mat_utils.h"
#include "math_utils.hpp"

using namespace libtempo;


uword mat_utils::find_nearest(const vec &a, double value) {
    return abs(a - value).index_min();
}

mat mat_utils::subtract_mean(const mat &tempogram) {
    auto ret = tempogram.each_row() - mean(tempogram, 0);
    return ret % (ret > 0);
}

mat mat_utils::rowwise_normalize(const mat &tempogram) {
    rowvec sums = sum(tempogram, 0);
    sums = sums + (sums == 0);
    return tempogram.each_row() / sums;
}

uvec mat_utils::argmax(const mat &tempogram) {
    uvec ret(tempogram.n_cols);
    for (uword i = 0; i < tempogram.n_cols; ++i) {
        ret[i] = tempogram(span::all, i).index_max();
    }
    return ret;
}

vec mat_utils::max_bucket(const mat &tempogram, const vec &axis_lut) {
    return axis_lut(argmax(tempogram));
}

vec mat_utils::pad_vec(const vec &x, unsigned int n_before, unsigned int n_after) {
    vec ret(x.size() + n_before + n_after, fill::zeros);
    ret(span((uword)n_before, n_before + x.size() - 1)) = x;
    return ret;
}

mat mat_utils::pad_mat(const mat &data, unsigned int n_before, unsigned int n_after, bool repeat) {
    mat band_krn(data.n_rows, data.n_cols + n_before + n_after);
    if(repeat) {
        for (int k = 0; k < n_before; ++k) {
            band_krn(span::all, (const uword) k) = data(span::all, 0);
        }

        for(int k = 0; k < n_after; ++k) {
            band_krn(span::all, band_krn.n_cols - 1 - k) = data(span::all, data.n_cols - 1);
        }
    }
    band_krn(span::all, span((const uword)n_before, band_krn.n_cols - 1 - n_after)) = data;

    return band_krn;
}

cx_mat mat_utils::colwise_normalize_p1(const cx_mat &feature, unsigned int p, double threshold) {
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

vec mat_utils::smooth_filter_subtract(const vec &novelty_curve, int sr, int hop_length) {
    double smooth_len = 1.5;
    smooth_len = fmax(ceil(smooth_len * sr / (double) hop_length), 3.);
    rowvec smooth_filter = (utils::math::my_hanning((const uword) smooth_len)).t();
    rowvec novelty_curve_t = novelty_curve.t();
    mat local_average = conv2(novelty_curve_t, flipud(fliplr(smooth_filter / sum(smooth_filter))), "same");

    rowvec novelty_sub = (novelty_curve_t - local_average);
    novelty_sub = novelty_sub % (novelty_sub > 0);

    return novelty_sub.t();
}
