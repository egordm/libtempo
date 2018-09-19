//
// Created by egordm on 29-8-2018.
//

#include "mat_utils.h"

uword tempogram::mat_utils::find_nearest(const vec &a, double value) {
    return abs(a - value).index_min();
}


mat tempogram::mat_utils::subtract_mean(const mat &tempogram) {
    auto ret = tempogram.each_row() - mean(tempogram, 0);
    return ret % (ret > 0);
}

mat tempogram::mat_utils::rowwise_normalize(const mat &tempogram) {
    rowvec sums = sum(tempogram, 0);
    sums = sums + (sums == 0);
    return tempogram.each_row() / sums;
}

uvec tempogram::mat_utils::argmax(const mat &tempogram) {
    uvec ret(tempogram.n_cols);
    for (uword i = 0; i < tempogram.n_cols; ++i) {
        ret[i] = tempogram(span::all, i).index_max();
    }
    return ret;
}

vec tempogram::mat_utils::max_bucket(const mat &tempogram, const vec &axis_lut) {
    return axis_lut(argmax(tempogram));
}

vec tempogram::mat_utils::pad_vec(const vec &x, unsigned int n_before, unsigned int n_after) {
    vec ret(x.size() + n_before + n_after, fill::zeros);
    ret(span((uword)n_before, n_before + x.size() - 1)) = x;
    return ret;
}
