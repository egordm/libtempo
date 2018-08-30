//
// Created by egordm on 29-8-2018.
//

#include "curve_utils.h"
#include <float.h>

using namespace tempogram;

vec curve_utils::correct_curve_by_length(const vec &measurements, int min_length) {
    // Split measurements in segments with same value
    auto segments = split_curve(measurements);

    // Take small segments and join if needed
    std::vector<uvec> small_segments;
    for (const auto &segment : segments) if (segment.n_rows < min_length) small_segments.push_back(segment);
    small_segments = join_adjacent_segments(small_segments);

    // Delete the small segments by replaing their value to neareast outside their boundaries
    vec ret(measurements);
    for (const auto &segment : small_segments) {
        double before = segment[0] > 0 ? measurements[segment[0] - 1] : DBL_MAX;
        double after = segment[segment.n_rows - 1] + 1 < measurements.n_rows
                       ? measurements[segment[segment.n_rows - 1] + 1] : DBL_MAX;
        double target = fabs(ret[segment[0]] - before) > fabs(ret[segment[0]] - after) ? after : before;
        if (target == DBL_MAX) continue;

        for (uword i : segment) ret[i] = target;
    }

    return ret;
}

vec curve_utils::correct_curve_by_confidence(const vec &measurements, const vec &confidence,
                                             float threshold) {
    vec ret(measurements);

    for (uword i = 0; i < measurements.n_rows; ++i) {
        // Pick confidence by averaging the two subsequent data points if possible.
        double conf = i + 1 < measurements.n_rows ? (confidence[i] + confidence[i + 1]) / 2 : confidence[i];
        if (conf < threshold) ret[i] = i == 0 ? -1 : ret[i - 1];
    }

    for (uword i = measurements.n_rows - 1; i >= 0; ++i) {
        if (ret[i] == -1) ret[i] = ret[i + 1];
    }

    return ret;
}

std::vector<uvec> curve_utils::split_curve(const vec &curve) {
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

std::vector<uvec> curve_utils::join_adjacent_segments(const std::vector<uvec> &segments) {
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

std::ostream &curve_utils::operator<<(std::ostream &os, const curve_utils::Section &section) {
    os << "Section: start: " << section.start << " end: " << section.end << " bpm: " << section.bpm
       << " offset: " << section.offset << " offset relative: " << (section.offset - section.start);
    return os;
}

std::vector<curve_utils::Section>
curve_utils::tempo_segments_to_sections(const std::vector<uvec> &segments, const vec &curve, const vec &t,
                                        double bpm_reference) {
    std::vector<curve_utils::Section> ret;

    for (const auto &segment : segments) {
        ret.emplace_back(t[segment[0]], t[segment[segment.n_rows - 1] + 1], curve[segment[0]] * bpm_reference);
    }

    return ret;
}

void curve_utils::split_section(const curve_utils::Section &section, std::vector<curve_utils::Section> &sections,
                                double max_section_len) {
    double length = section.end - section.start;
    if(length < max_section_len) {
        sections.push_back(section);
        return;
    }

    Section part1 = section;
    part1.end = section.start + length/2;
    split_section(part1, sections, max_section_len);

    Section part2 = section;
    part2.start = section.start + length/2;
    split_section(part2, sections, max_section_len);
}
