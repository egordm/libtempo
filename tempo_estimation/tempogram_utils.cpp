//
// Created by egordm on 29-8-2018.
//

#include "tempogram_utils.h"
#include "generic_algothms.h"
#include "math_utils.hpp"

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

mat tempogram::tempogram_utils::tempogram_apply_window(const mat &tempogram, const vec &window) {
    int window_length = (int) window.n_rows;
    int n_bins = (int) tempogram.n_rows;
    mat padding((uword)n_bins, (uword)(window_length/2), fill::zeros);
    mat padded_tempogram = join_rows(padding, tempogram);
    padded_tempogram = join_rows(padded_tempogram, padding);
    rowvec col_window = window.t();

    mat ret(arma::size(tempogram), fill::zeros);
    for (uword i = 0; i < tempogram.n_cols; ++i) {
        auto roi = padded_tempogram(span::all, span(i, i + window_length - 1));
        roi = roi.each_row() % col_window;
        ret(span::all, i) = sum(roi, 1);
    }

    return ret;
}

mat tempogram::tempogram_utils::tempogram_include_triplets(const mat &tempogram, const vec &axis_lut, float weight) {
    double triplet_fraction = 3 / 2.;
    mat ret(arma::size(tempogram));

    for (uword k = 0; k < axis_lut.n_rows; ++k) {
        // Calculate triplet height on the tempogram. TODO: assuming cyclic tempogram with 1 - 2 axis
        double val = std::fmod((triplet_fraction * axis_lut[k]) - 1, 2.) + 1;
        uword i = find_nearest(axis_lut, val);

        // Weight triplets by its base to avoid adding nonexistent triplet intensities
        ret(k, span::all) = tempogram(k, span::all) + tempogram(i, span::all) % tempogram(k, span::all) * weight;
    }

    return ret;
}

mat tempogram::tempogram_utils::smoothen_tempogram(const mat &tempogram, const vec &axis_lut, int temporal_unit_size,
                                                   float triplet_weight) {
    mat tripcor = tempogram_include_triplets(tempogram, axis_lut, triplet_weight);
    vec window_hann = utils::math::my_hanning(static_cast<const uword>(temporal_unit_size));
    vec window_const(static_cast<const uword>(temporal_unit_size), fill::ones);

    mat ret = tempogram_apply_window(tripcor, window_const);
    ret += tempogram_apply_window(tripcor, window_hann);
    ret = normalize_tempogram(subtract_mean(ret));
    return ret;
}

vec tempogram::tempogram_utils::extract_tempo_curve(const mat &tempogram, const vec &axis_lut) {
    return max_bucket(tempogram, axis_lut);
}

double tempogram::tempogram_utils::bpm_to_cyclic(double bpm, double ref_tempo) {
    double mins = floor(log2(bpm / 60));
    double diff = ((ref_tempo * pow(2, mins + 1)) - (ref_tempo * pow(2, mins)));
    return  (bpm - (60 * pow(2, mins))) / diff + 1;
}
