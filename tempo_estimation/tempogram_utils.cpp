//
// Created by egordm on 29-8-2018.
//

#include "tempogram_utils.h"
#include "generic_algothms.h"

using namespace tempogram::generic_algorithms;

mat tempogram::tempogram_utils::subtract_mean(const mat &tempogram) {
    auto ret = tempogram.each_row() - mean(tempogram, 0);
    return ret % (ret > 0);
}

mat tempogram::tempogram_utils::normalize_tempogram(const mat &tempogram) {
    rowvec sums = sum(tempogram, 0);
    sums = sums + (sums == 0);
    return tempogram.each_row() / sums;
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

mat tempogram::tempogram_utils::tempogram_include_triplets(const mat &tempogram, const vec &axis_lut, float weight) {
    double triplet_fraction = 3 / 2.;
    mat ret(size(tempogram));

    for (uword k = 0; k < axis_lut.n_rows; ++k) {
        // Calculate triplet height on the tempogram. TODO: assuming cyclic tempogram with 1 - 2 axis
        double val = std::fmod((triplet_fraction * axis_lut[k]) - 1, 2.) + 1;
        uword i = find_nearest(axis_lut, val);

        // TODO instead of add. multiply by base intensity first. so 0 -> 0 and not 0 -> 1. So no ghost triplets appear
        ret(k, span::all) = tempogram(k, span::all) + tempogram(i, span::all) * weight;
    }

    return ret;
}

mat tempogram::tempogram_utils::smoothen_tempogram(const mat &tempogram, const vec &axis_lut, int temporal_unit_size,
                                                   float triplet_weight) {
    mat ret = tempogram_include_triplets(tempogram, axis_lut, triplet_weight);
    ret = accumulate_temporal(ret, temporal_unit_size);
    ret = normalize_tempogram(subtract_mean(ret));
    return ret;
}

vec tempogram::tempogram_utils::extract_tempo_curve(const mat &tempogram, const vec &axis_lut) {
    return max_bucket(tempogram, axis_lut);
}
