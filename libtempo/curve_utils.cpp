//
// Created by egordm on 29-8-2018.
//

#include "curve_utils.h"
#include "signal_utils.h"
#include <float.h>

using namespace libtempo;
using namespace libtempo::signal_utils;

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

    ret.reserve(segments.size());
    for (const auto &segment : segments) {
        double end = (segment[segment.n_rows - 1] + 1 < t.n_rows)
                     ? t[segment[segment.n_rows - 1] + 1] : t[segment[segment.n_rows - 1]];
        ret.emplace_back(t[segment[0]], end, curve[segment[0]] * bpm_reference);
    }

    return ret;
}

void curve_utils::split_section(const curve_utils::Section &section, std::vector<curve_utils::Section> &sections,
                                double max_section_len) {
    double length = section.end - section.start;
    if (length < max_section_len) {
        sections.push_back(section);
        return;
    }

    Section part1 = section;
    part1.end = section.start + length / 2;
    split_section(part1, sections, max_section_len);

    Section part2 = section;
    part2.start = section.start + length / 2;
    split_section(part2, sections, max_section_len);
}

void curve_utils::extract_offset(const vec &novelty_curve, curve_utils::Section &section,
                                 const std::vector<int> &tempo_multiples, int feature_rate, float bpm_doubt_window,
                                 double bpm_doubt_step) {
    int start = static_cast<int>(section.start * feature_rate);
    int end = static_cast<int>(section.end * feature_rate);
    int window_length = end - start;

    double min_bpm = section.bpm - bpm_doubt_window / 2;
    int doubt_bpm_step_count = static_cast<int>(bpm_doubt_window / bpm_doubt_step);
    std::vector<std::tuple<double, double, double>> candidates;
    candidates.resize(static_cast<unsigned long long int>(doubt_bpm_step_count));

#pragma omp parallel for
    for (int b = 0; b < doubt_bpm_step_count; ++b) {
        double bpm = min_bpm + b * bpm_doubt_step;
        double b_offset = 0;
        double b_bpm = 0;
        double b_offset_magnitude = -DBL_MAX;

        // Bar has 4 qtr notes which are the definition of bpm
        int samples_per_bar = (int) ceil(60. / bpm * feature_rate) * 4;

        std::vector<std::tuple<vec, vec>> pulses; // Create a list fo pulses for given bpm multiples
        pulses.reserve(tempo_multiples.size());
        for (const int &multiple : tempo_multiples) {
            pulses.push_back(generate_pulse(bpm * multiple, window_length + samples_per_bar, feature_rate));
        }

        vec roi = novelty_curve(span((uword) start, (uword) end - 1));
        for (int i = 0; i < samples_per_bar; ++i) {
            double magnitude = 0;

            for (const auto &pulse : pulses) {
                vec co = roi % std::get<0>(pulse)(span((uword) i, (uword) i + window_length - 1));
                magnitude += sum(co % (co > 0));
            }

            if (magnitude > b_offset_magnitude) {
                b_offset_magnitude = magnitude;
                b_offset = i;
                b_bpm = bpm;
            }
        }

        candidates[b] = std::make_tuple(b_offset_magnitude, b_offset, b_bpm);
    }

    const auto &candidate = *std::max_element(candidates.begin(), candidates.end(),
                                              [](const std::tuple<double, double, double> &l,
                                                 const std::tuple<double, double, double> &r) {
                                                  return std::get<0>(l) < std::get<0>(r);
                                              });

    section.offset = -std::get<1>(candidate) / feature_rate + section.start;
    section.bpm = std::get<2>(candidate);

}

void curve_utils::correct_offset(curve_utils::Section &section, int smallest_fraction_shift) {
    double offset = section.offset - section.start;
    double bar_len = 60. / section.bpm * 4;
    double fraction_note_len = bar_len / smallest_fraction_shift;

    if (offset < 0) offset += ceil(fabs(offset) / bar_len) * bar_len;
    offset = std::fmod(offset, fraction_note_len);

    section.offset = section.start + offset;
}

std::vector<curve_utils::Section>
curve_utils::merge_sections(const std::vector<curve_utils::Section> &sections, float threshold) {
    if (sections.empty()) return sections;
    std::vector<curve_utils::Section> ret;

    std::vector<curve_utils::Section> candidates = {sections[0]};
    for (int i = 1; i < sections.size(); ++i) {
        double diff = fabs(candidates[0].bpm - sections[i].bpm);
        if (diff > threshold) {
            ret.push_back(average_sections(candidates));
            candidates.clear();
        }

        candidates.push_back(sections[i]);
    }

    ret.push_back(average_sections(candidates));

    return ret;
}

curve_utils::Section curve_utils::average_sections(const std::vector<curve_utils::Section> &sections) {
    curve_utils::Section ret(sections.front().start, sections.back().end, 0, 0);

    for (const auto &candidate : sections) {
        ret.bpm += candidate.bpm * (candidate.end - candidate.start);
        // TODO: should be done by makign it relative to the bar length or 1/4th note length
        ret.offset += candidate.offset * (candidate.end - candidate.start);
    }
    ret.bpm /= (ret.end - ret.start);
    ret.offset /= (ret.end - ret.start);

    return ret;
}
