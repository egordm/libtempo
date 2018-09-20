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

namespace libtempo_wrapper {
    template<typename T>
    struct MatrixWrapper {
        Mat<T> data;

        using ArrayType = py::array_t<T, py::array::f_style | py::array::forcecast>;

        explicit MatrixWrapper(Mat<T> data) : data(std::move(data)) {}

        std::tuple<unsigned long, unsigned long> get_size() { return std::make_tuple(data.n_rows, data.n_cols); }

        ArrayType to_array() {
            return arma_to_py(data);
        }
    };

    template<typename T>
    inline MatrixWrapper<T> wrap_array(py::array_t<T, py::array::f_style | py::array::forcecast> &array) {
        auto data = py_to_arma_mat<T>(array);
        return MatrixWrapper<T>(data);
    }

    inline std::tuple<MatrixWrapper<double>, int>
    audio_to_novelty_curve(MatrixWrapper<float> signal, int sr, int window_length = -1, int hop_length = -1,
                           double compression_c = 1000, bool log_compression = true, int resample_feature_rate = 200) {
        int feature_rate;
        auto novelty_curve = tempogram_processing::audio_to_novelty_curve
                (feature_rate, signal.data, sr, window_length, hop_length, compression_c, log_compression,
                 resample_feature_rate);

        return std::make_tuple(MatrixWrapper(novelty_curve), feature_rate);
    }

    inline std::tuple<MatrixWrapper<cx_double>, MatrixWrapper<double>, MatrixWrapper<double>>
    novelty_curve_to_tempogram(const MatrixWrapper<double> &novelty_curve, const MatrixWrapper<double> &bpm,
                               double feature_rate, int tempo_window, int hop_length) {
        vec t;
        auto tempogram = tempogram_processing::novelty_curve_to_tempogram_dft
                (t, novelty_curve.data, bpm.data, feature_rate, tempo_window, hop_length);
        auto normalized_tempogram = mat_utils::colwise_normalize_p1(tempogram, 2, 0.0001);

        return std::make_tuple(MatrixWrapper(normalized_tempogram), MatrixWrapper(bpm), MatrixWrapper(t));
    }

    inline std::tuple<MatrixWrapper<double>, MatrixWrapper<double>>
    tempogram_to_cyclic_tempogram(const MatrixWrapper<cx_double> &tempogram, const MatrixWrapper<double> &bpm,
                                  int octave_divider = 30, int ref_tempo = 60) {
        vec y_axis;
        auto cyclic_tempogram = tempogram_processing::tempogram_to_cyclic_tempogram
                (y_axis, tempogram.data, bpm.data, octave_divider, ref_tempo);

        return std::make_tuple(MatrixWrapper(cyclic_tempogram), MatrixWrapper(y_axis));
    }

    inline MatrixWrapper<double>
    smoothen_tempogram(const MatrixWrapper<double> &tempogram, const MatrixWrapper<double> &y_axis,
                       const MatrixWrapper<double> &t, double temporal_unit_size = 20, float triplet_weight = 0.8f) {
        int smooth_length_samples = (int) (temporal_unit_size / (t.data.at(1) - t.data.at(0)));
        auto smooth_tempogram = tempogram_utils::smoothen_tempogram
                (tempogram.data, y_axis.data, smooth_length_samples, triplet_weight);

        return MatrixWrapper(smooth_tempogram);
    }

    inline MatrixWrapper<double>
    tempogram_to_tempo_curve(const MatrixWrapper<double> &tempogram, const MatrixWrapper<double> &y_axis) {
        auto tempo_curve = tempogram_utils::extract_tempo_curve(tempogram.data, y_axis.data);
        return MatrixWrapper(tempo_curve);
    }

    inline MatrixWrapper<double>
    correct_tempo_curve(const MatrixWrapper<double> &tempo_curve, const MatrixWrapper<double> &t, double min_section_length = 10) {
        int min_section_length_samples = (int) (min_section_length / (t.data.at(1) - t.data.at(0)));
        auto corrected_curve = curve_utils::correct_curve_by_length(tempo_curve.data, min_section_length_samples);
        return MatrixWrapper(corrected_curve);
    }

    inline std::vector<curve_utils::Section>
    curve_to_sections(const MatrixWrapper<double> &curve, const MatrixWrapper<double> &t,
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
    sections_extract_offset(const MatrixWrapper<double> &novelty_curve, std::vector<curve_utils::Section> sections,
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
