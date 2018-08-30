//
// Created by egordm on 28-7-2018.
//

#ifndef PROJECT_TEMPOGRAM_WRAPPER_H
#define PROJECT_TEMPOGRAM_WRAPPER_H

#include "pyarma.hpp"
#include <tempo_estimation.h>
#include <helper_functions.h>
#include <tempogram_utils.h>
#include <curve_utils.h>

using namespace tempogram;

namespace tempogram_wrapper {
    inline std::tuple<py::array, py::array, py::array>
    novelty_curve_to_tempogram_dft(pyarr_d novelty_curve_np, pyarr_d bpm_np, double feature_rate, int tempo_window,
                                   int hop_length) {
        arma::vec novelty_curve = py_to_arma_vec<double>(novelty_curve_np);
        arma::vec bpm = py_to_arma_vec<double>(bpm_np);
        auto ret = tempogram::novelty_curve_to_tempogram_dft(novelty_curve, bpm, feature_rate, tempo_window,
                                                             hop_length);

        return std::make_tuple(arma_to_py(std::get<0>(ret)),
                               arma_to_py(std::get<1>(ret)),
                               arma_to_py(std::get<2>(ret)));
    }

    inline py::array normalize_feature(pyarr_cd &feature_np, unsigned int p, double threshold) {
        auto feature = py_to_arma_mat<cx_double>(feature_np);
        auto ret = tempogram::normalize_feature(feature, p, threshold);
        return arma_to_py(ret);
    }

    inline std::tuple<py::array, int>
    audio_to_novelty_curve(pyarr_d signal_np, int sr, int window_length = -1, int hop_length = -1,
                           double compression_c = 1000, bool log_compression = true,
                           int resample_feature_rate = 200) {
        arma::vec signal = py_to_arma_vec<double>(signal_np);
        auto ret = tempogram::audio_to_novelty_curve(signal, sr, window_length, hop_length, compression_c,
                                                     log_compression,
                                                     resample_feature_rate);

        return std::make_tuple(arma_to_py(std::get<0>(ret)), std::get<1>(ret));
    };

    inline std::tuple<py::array, int, py::array, py::array, py::array>
    audio_to_novelty_curve_tempogram(pyarr_d signal_np, int sr, pyarr_d bpm_np, int tempo_window, int hop_length) {
        arma::vec signal = py_to_arma_vec<double>(signal_np);
        arma::vec bpm = py_to_arma_vec<double>(bpm_np);

        auto nov_cv = tempogram::audio_to_novelty_curve(signal, sr);
        auto ret = tempogram::novelty_curve_to_tempogram_dft(std::get<0>(nov_cv), bpm, std::get<1>(nov_cv),
                                                             tempo_window,
                                                             hop_length);
        auto normalized_tempogram = tempogram::normalize_feature(std::get<0>(ret), 2, 0.0001);

        return std::make_tuple(
                arma_to_py(std::get<0>(nov_cv)), // novelty curve
                std::get<1>(nov_cv), // novelty curve feature rate
                arma_to_py(normalized_tempogram), // tempogram
                arma_to_py(std::get<1>(ret)), // tempogram frequencies
                arma_to_py(std::get<2>(ret))); // tempogram times
    };

    inline std::tuple<py::array, py::array>
    tempogram_to_cyclic_tempogram(pyarr_cd tempogram_np, pyarr_d bpm_np, int octave_divider = 30, int ref_tempo = 60) {
        arma::cx_mat tempogram = py_to_arma_mat<cx_double>(tempogram_np);
        arma::vec bpm = py_to_arma_vec<double>(bpm_np);

        auto ret = tempogram::tempogram_to_cyclic_tempogram(tempogram, bpm, octave_divider, ref_tempo);

        return std::make_tuple(arma_to_py(std::get<0>(ret)), arma_to_py(std::get<1>(ret)));
    }

    inline py::array
    smoothen_tempogram(pyarr_d tempogram_np, pyarr_d axis_lut_np, int temporal_unit_size = 100,
                       float triplet_weight = 0.8f) {
        arma::mat tempogram = py_to_arma_mat<double>(tempogram_np);
        arma::vec axis_lut = py_to_arma_vec<double>(axis_lut_np);

        auto smooth_tempogram = tempogram::tempogram_utils::smoothen_tempogram(tempogram, axis_lut, temporal_unit_size,
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

        for(auto &section : sections) {
            curve_utils::extract_offset(novelty_curve, section,tempo_multiples, feature_rate, bpm_doubt_window, bpm_doubt_step);
        }

        return sections;
    }

}


#endif //PROJECT_TEMPOGRAM_WRAPPER_H
