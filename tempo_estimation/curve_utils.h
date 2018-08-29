//
// Created by egordm on 29-8-2018.
//

#ifndef PROJECT_CURVE_UTILS_H
#define PROJECT_CURVE_UTILS_H

#include <armadillo>
#include <ostream>
#include "defines.h"

using namespace arma;

namespace tempogram { namespace curve_utils {
    /**
     * Splits curve in segments with all the uniform value.
     * A returend segments contains indices of the points on the curve.
     *
     * @param curve
     * @return
     */
    std::vector<uvec> split_curve(const vec &curve);

    /**
     * Joins adjacent segments
     *
     * @param segments
     * @return
     */
    std::vector<uvec> join_adjacent_segments(const std::vector<uvec> &segments);

    /**
     * Correct curve by removing short value changes and thus removing small sudden spikes.
     *
     * @param measurements
     * @param min_length
     * @return
     */
    vec correct_curve_by_length(const vec &measurements, int min_length);

    /**
     * Correct curve by ignoring measurements which have low confidence
     *
     * TODO: Lots of room for optimization.
     * @param measurements
     * @param confidence
     * @param threshold
     * @return
     */
    vec correct_curve_by_confidence(const vec &measurements, const vec &confidence, float threshold = 0.85);

    struct Section {
        double start, end, bpm, offset;

        Section(double start, double end, double bpm, double offset = 0)
                : start(start), end(end), bpm(bpm), offset(offset) {}

        friend std::ostream &operator<<(std::ostream &os, const Section &section);
    };

    std::vector<Section> tempo_segments_to_sections(const std::vector<uvec> &segments, const vec &curve, const vec &t,
            double bpm_reference = DEFAULT_REF_TEMPO);

}}


#endif //PROJECT_CURVE_UTILS_H
