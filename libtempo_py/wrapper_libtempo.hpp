#include <utility>

//
// Created by egordm on 28-7-2018.
//

#ifndef PROJECT_TEMPOGRAM_WRAPPER_H
#define PROJECT_TEMPOGRAM_WRAPPER_H

#include "pyarma.hpp"
#include <tempogram_processing.h>
#include <mat_utils.h>
#include <tempogram_utils.h>
#include <curve_utils.h>

using namespace libtempo;

namespace wrapper_libtempo {
    inline std::tuple<MatWD, int>
    audio_to_novelty_curve(MatWF signal, int sr, int window_length = -1, int hop_length = -1,
                           double compression_c = 1000, bool log_compression = true, int resample_feature_rate = 200) {
        int feature_rate;
        auto novelty_curve = tempogram_processing::audio_to_novelty_curve
                (feature_rate, signal.data, sr, window_length, hop_length, compression_c, log_compression,
                 resample_feature_rate);

        return std::make_tuple(MatWD(novelty_curve), feature_rate);
    }

    inline std::tuple<MatWCD, MatWD>
    novelty_curve_to_tempogram(const MatWD &novelty_curve, const MatWD &bpm, double feature_rate, int tempo_window,
                               int hop_length) {
        vec t;
        auto tempogram = tempogram_processing::novelty_curve_to_tempogram_dft
                (t, novelty_curve.data, bpm.data, feature_rate, tempo_window, hop_length);
        auto normalized_tempogram = mat_utils::colwise_normalize_p1(tempogram, 2, 0.0001);

        return std::make_tuple(MatWCD(normalized_tempogram), MatWD(t));
    }

    inline std::tuple<MatWD, MatWD>
    tempogram_to_cyclic_tempogram(const MatWCD &tempogram, const MatWD &bpm, int octave_divider = 30,
                                  int ref_tempo = 60) {
        vec y_axis;
        auto cyclic_tempogram = tempogram_processing::tempogram_to_cyclic_tempogram
                (y_axis, tempogram.data, bpm.data, octave_divider, ref_tempo);

        return std::make_tuple(MatWD(cyclic_tempogram), MatWD(y_axis));
    }

    inline MatWD
    smoothen_tempogram(const MatWD &tempogram, const MatWD &y_axis, const MatWD &t, double temporal_unit_size = 20,
                       float triplet_weight = 0.8f) {
        int smooth_length_samples = (int) (temporal_unit_size / (t.data.at(1) - t.data.at(0)));
        auto smooth_tempogram = tempogram_utils::smoothen_tempogram
                (tempogram.data, y_axis.data, smooth_length_samples, triplet_weight);

        return MatWD(smooth_tempogram);
    }

    inline MatWD
    tempogram_to_tempo_curve(const MatWD &tempogram, const MatWD &y_axis) {
        auto tempo_curve = tempogram_utils::extract_tempo_curve(tempogram.data, y_axis.data);
        return MatWD(tempo_curve);
    }

    inline MatWD
    correct_tempo_curve(const MatWD &tempo_curve, const MatWD &t, double min_section_length = 10) {
        int min_section_length_samples = (int) (min_section_length / (t.data.at(1) - t.data.at(0)));
        auto corrected_curve = curve_utils::correct_curve_by_length(tempo_curve.data, min_section_length_samples);
        return MatWD(corrected_curve);
    }

    inline std::vector<curve_utils::Section>
    curve_to_sections(const MatWD &curve, const MatWD &t,
                      double bpm_reference = DEFAULT_REF_TEMPO, double max_section_size = 60,
                      float bpm_merge_threshold = 0.5f) {
        auto segments = curve_utils::split_curve(curve.data);
        auto sections_tmp = curve_utils::tempo_segments_to_sections(segments, curve.data, t.data, bpm_reference);
        // Merge sections for consistency
        sections_tmp = curve_utils::merge_sections(sections_tmp, bpm_merge_threshold);
        // Split section for precision
        std::vector<curve_utils::Section> sections;
        for (const auto &section : sections_tmp) curve_utils::split_section(section, sections, max_section_size);

        return sections;
    }

    inline std::vector<curve_utils::Section>
    sections_extract_offset(const MatWD &novelty_curve, std::vector<curve_utils::Section> sections,
                            const std::vector<int> &tempo_multiples, int feature_rate, float bpm_doubt_window = 2,
                            double bpm_doubt_step = 0.1) {
        for (auto &section : sections) {
            curve_utils::extract_offset(novelty_curve.data, section, tempo_multiples, feature_rate, bpm_doubt_window,
                                        bpm_doubt_step);
        }

        return sections;
    }

}


#endif //PROJECT_TEMPOGRAM_WRAPPER_H
