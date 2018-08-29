//
// Created by egordm on 29-8-2018.
//

#include "tempogram_utils.h"

mat tempogram::tempogram_utils::subtract_mean(const mat &tempogram) {
    auto ret = tempogram - mean(tempogram, 0);
    return ret * (ret > 0);
}

mat tempogram::tempogram_utils::normalize(const mat &tempogram) {
    vec sums = sum(tempogram, 0);
    sums = sums + (sums == 0);
    return tempogram / sums;
}

uvec tempogram::tempogram_utils::argmax(const mat &tempogram) {
    uvec ret(tempogram.n_cols);
    for (uword i = 0; i < tempogram.n_cols; ++i) {
        ret[i] = tempogram(span::all, i).index_max();
    }
    return ret;
}

vec tempogram::tempogram_utils::max_bucket(const mat &tempogram, const vec &axis_lut) {
    return axis_lut(argmax(tempogram));
}

vec tempogram::tempogram_utils::extract_confidence(const mat &tempogram, const bool &rollover) {
    rowvec median = sum(tempogram, 0) / 2;
    uvec maxes = argmax(tempogram);
    int n_bins = (int) tempogram.n_rows;
    int half_bins = (int) tempogram.n_rows / 2;
    vec ret(tempogram.n_cols, fill::ones);

    for (uword i = 0; i < tempogram.n_cols; ++i) {
        double acc = tempogram(maxes[i], i);

        if (acc >= median[i]) {
            ret[i] = 0;
            continue;
        }

        for (int k = 1; k < half_bins; ++k) {
            uword upper = rollover ? (maxes[i] + k) % n_bins : maxes[i] + k;
            uword lower = maxes[i] - k;
            if (rollover && lower < 0) lower = n_bins + lower;

            if (upper >= 0 && upper < n_bins) acc += tempogram(upper, i);
            if (upper >= 0 && lower < n_bins) acc += tempogram(lower, i);

            if (acc >= median[i]) {
                ret[i] = k / (double) half_bins;
                break;
            }
        }
    }

    return 1 - ret;
}

mat tempogram::tempogram_utils::accumulate_temporal(const mat &tempogram, int temporal_size) {
    int n_bins = (int) tempogram.n_rows;
    mat ret((const uword) n_bins, tempogram.n_cols + temporal_size, fill::zeros);

    for (int i = 0; i < temporal_size; ++i) {
        mat pad_before((uword) n_bins, (uword) i, fill::zeros);
        mat pad_after((uword) n_bins, (uword) (temporal_size - i), fill::zeros);
        mat padded_tempogram = join_rows(pad_before, tempogram);
        padded_tempogram = join_rows(padded_tempogram, pad_after);
        ret += padded_tempogram;
    }

    return ret(span::all, span((const uword)temporal_size, ret.n_cols - temporal_size - 1));
}
