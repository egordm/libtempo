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

namespace libtempo_wrapper {
    inline std::tuple<py::array, py::array, py::array>
    novelty_curve_to_tempogram_dft(pyarr_d novelty_curve_np, pyarr_d bpm_np, double feature_rate, int tempo_window,
                                   int hop_length) {
        arma::vec novelty_curve = py_to_arma_vec<double>(novelty_curve_np);
        arma::vec bpm = py_to_arma_vec<double>(bpm_np);
        vec t;
        auto tempogram = tempogram_processing::novelty_curve_to_tempogram_dft(t, novelty_curve, bpm, feature_rate,
                                                                              tempo_window,
                                                                              hop_length);

        return std::make_tuple(arma_to_py(tempogram), arma_to_py(bpm), arma_to_py(t));
    }

    inline py::array normalize_feature(pyarr_cd &feature_np, unsigned int p, double threshold) {
        auto feature = py_to_arma_mat<cx_double>(feature_np);
        auto ret = mat_utils::colwise_normalize_p1(feature, p, threshold);
        return arma_to_py(ret);
    }

    inline std::tuple<py::array, int>
    audio_to_novelty_curve(pyarr_f signal_np, int sr, int window_length = -1, int hop_length = -1,
                           double compression_c = 1000, bool log_compression = true,
                           int resample_feature_rate = 200) {
        arma::fvec signal = py_to_arma_vec<float>(signal_np);
        int feature_rate;
        auto novelty_curve = tempogram_processing::audio_to_novelty_curve
                (feature_rate, signal, sr, window_length, hop_length, compression_c, log_compression,
                 resample_feature_rate);

        return std::make_tuple(arma_to_py(novelty_curve), feature_rate);
    };

    inline std::tuple<py::array, int, py::array, py::array, py::array>
    audio_to_novelty_curve_tempogram(pyarr_f signal_np, int sr, pyarr_d bpm_np, int tempo_window, int hop_length) {
        arma::fvec signal = py_to_arma_vec<float>(signal_np);
        arma::vec bpms = py_to_arma_vec<double>(bpm_np);
        int feature_rate;
        vec t;

        auto novelty_curve = tempogram_processing::audio_to_novelty_curve(feature_rate, signal, sr);
        auto tempogram = tempogram_processing::novelty_curve_to_tempogram_dft(t, novelty_curve, bpms, feature_rate,
                                                                              tempo_window, hop_length);
        auto normalized_tempogram = mat_utils::colwise_normalize_p1(tempogram, 2, 0.0001);

        return std::make_tuple(arma_to_py(novelty_curve), feature_rate, arma_to_py(normalized_tempogram),
                               arma_to_py(bpms), arma_to_py(t));
    };

    inline std::tuple<py::array, py::array>
    tempogram_to_cyclic_tempogram(pyarr_cd tempogram_np, pyarr_d bpm_np, int octave_divider = 30, int ref_tempo = 60) {
        arma::cx_mat tempogram = py_to_arma_mat<cx_double>(tempogram_np);
        arma::vec bpm = py_to_arma_vec<double>(bpm_np);

        vec y_axis;
        auto cyclic_tempogram = tempogram_processing::tempogram_to_cyclic_tempogram
                (y_axis, tempogram, bpm, octave_divider, ref_tempo);

        return std::make_tuple(arma_to_py(cyclic_tempogram), arma_to_py(y_axis));
    }

    inline py::array
    smoothen_tempogram(pyarr_d tempogram_np, pyarr_d axis_lut_np, int temporal_unit_size = 100,
                       float triplet_weight = 0.8f) {
        arma::mat tempogram = py_to_arma_mat<double>(tempogram_np);
        arma::vec axis_lut = py_to_arma_vec<double>(axis_lut_np);

        auto smooth_tempogram = tempogram_utils::smoothen_tempogram(tempogram, axis_lut, temporal_unit_size,
                                                                    triplet_weight);
        return arma_to_py(smooth_tempogram);
    }

    inline py::array
    tempogram_to_tempo_curve_corrected(pyarr_d tempogram_np, pyarr_d axis_lut_np, int min_length = 40) {
        arma::mat tempogram = py_to_arma_mat<double>(tempogram_np);
        arma::vec axis_lut = py_to_arma_vec<double>(axis_lut_np);

        auto tempo_curve = tempogram_utils::extract_tempo_curve(tempogram, axis_lut);
        tempo_curve = curve_utils::correct_curve_by_length(tempo_curve, min_length);

        return arma_to_py(tempo_curve);
    }

    inline std::vector<curve_utils::Section>
    curve_to_sections(pyarr_d curve_np, pyarr_d t_np, double bpm_reference = DEFAULT_REF_TEMPO,
                      double max_section_size = 60) {
        arma::vec curve = py_to_arma_vec<double>(curve_np);
        arma::vec t = py_to_arma_vec<double>(t_np);

        auto segments = curve_utils::split_curve(curve);
        auto sections_tmp = curve_utils::tempo_segments_to_sections(segments, curve, t, bpm_reference);
        std::vector<curve_utils::Section> sections;
        for (const auto &section : sections_tmp) curve_utils::split_section(section, sections, max_section_size);

        return sections;
    }

    inline std::vector<curve_utils::Section>
    sections_extract_offset(pyarr_d novelty_curve_np, std::vector<curve_utils::Section> sections,
                            const std::vector<int> &tempo_multiples, int feature_rate, float bpm_doubt_window = 2,
                            double bpm_doubt_step = 0.1) {
        arma::vec novelty_curve = py_to_arma_vec<double>(novelty_curve_np);

        for (auto &section : sections) {
            curve_utils::extract_offset(novelty_curve, section, tempo_multiples, feature_rate, bpm_doubt_window,
                                        bpm_doubt_step);
        }

        return sections;
    }

}


#endif //PROJECT_TEMPOGRAM_WRAPPER_H
