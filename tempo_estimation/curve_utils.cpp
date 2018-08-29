//
// Created by egordm on 29-8-2018.
//

#include "curve_utils.h"
#include <float.h>

std::vector<uvec> tempogram::curve_utils::split_curve(const vec &curve) {
    std::vector<uvec> ret;
    std::vector<uword> current_section = {0};
    double current_section_value = curve[current_section[0]];

    for (uword i = 1; i < curve.n_rows; ++i) {
        if (current_section_value == curve[i]) current_section.push_back(i);
        else {
            ret.emplace_back(current_section);
            current_section = {i};
            current_section_value = curve[i];
        }
    }

    if (!current_section.empty()) ret.emplace_back(current_section);

    return ret;
}

std::vector<uvec> tempogram::curve_utils::join_adjacent_segments(const std::vector<uvec> &segments) {
    std::vector<uvec> ret;

    for (const auto &segment : segments) {
        uword lst_idx = ret.size() - 1;
        if (!ret.empty() && ret[lst_idx][ret[lst_idx].n_rows - 1] == segment[0] - 1) {
            ret[lst_idx] = join_cols(ret[lst_idx], segment);
        } else {
            ret.push_back(segment);
        }
    }

    return ret;
}

vec tempogram::curve_utils::correct_curve_by_length(const vec &measurements, int min_length) {
    // Split measurements in segments with same value
    auto segments = split_curve(measurements);

    // Take small segments and join if needed
    std::vector<uvec> small_segments;
    for (const auto &segment : segments) if (segment.n_rows < min_length) small_segments.push_back(segment);
    small_segments = join_adjacent_segments(small_segments);

    // Delete the small segments by replaing their value to neareast outside their boundaries
    vec ret(measurements);
    for (const auto &segment : small_segments) {
        double before = segment[0] - 1 > 0 ? measurements[segment[0] - 1] : DBL_MAX;
        double after = measurements[segment[segment.n_rows - 1] + 1] > measurements.n_rows
                       ? measurements[segment[segment.n_rows - 1] + 1] : DBL_MAX;
        double target = fabs(ret[segment[0]] - before) > fabs(ret[segment[0]] - after) ? after : before;
        if (target == DBL_MAX) continue;

        for (uword i : segment) ret[i] = target;
    }

    return ret;
}

vec tempogram::curve_utils::correct_curve_by_confidence(const vec &measurements, const vec &confidence,
                                                        float threshold) {
    vec ret(measurements);

    for(uword i = 0; i < measurements.n_rows; ++i) {
        // Pick confidence by averaging the two subsequent data points if possible.
        double conf = i + 1 < measurements.n_rows ? (confidence[i] + confidence[i + 1]) / 2 : confidence[i];
        if(conf < threshold) ret[i] = i == 0 ? -1 : ret[i - 1];
    }

    for(uword i = measurements.n_rows - 1; i >= 0; ++i) {
        if(ret[i] == -1) ret[i] = ret[i + 1];
    }

    return ret;
}
