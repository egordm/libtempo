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
    for(uword i = 0; i < tempogram.n_cols; ++i) {
        ret[i] = tempogram(span::all, i).index_max();
    }
    return ret;
}

vec tempogram::tempogram_utils::max_bucket(const mat &tempogram, const vec &axis_lut) {
    return axis_lut(argmax(tempogram));
}

vec tempogram::tempogram_utils::extract_confidence(const mat &tempogram, const bool &rollover) {
    rowvec median = sum(tempogram, 0) / 2;
    std::cout << median << std::endl;
    uvec maxes = argmax(tempogram);
    int n_bins = (int) tempogram.n_rows;
    int half_bins = (int) tempogram.n_rows / 2;
    vec ret(tempogram.n_cols, fill::ones);

    for(uword i = 0; i < tempogram.n_cols; ++i) {
        double acc = tempogram(maxes[i], i);

        if(acc >= median[i]) {
            ret[i] = 0;
            continue;
        }

        for(int k = 1; k < half_bins; ++k) {
            uword upper = rollover ? (maxes[i] + k) % n_bins : maxes[i] + k;
            uword lower = maxes[i] - k;
            if(rollover && lower < 0) lower = n_bins + lower;

            if(upper >= 0 && upper < n_bins) acc += tempogram(upper, i);
            if(upper >= 0 && lower < n_bins) acc += tempogram(lower, i);

            if(acc >= median[i]) {
                ret[i] = k / (double)half_bins;
                break;
            }
        }
    }

    return 1 - ret;
}
