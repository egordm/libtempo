//
// Created by egordm on 21-9-2018.
//

#ifndef PROJECT_MODULE_LIBTEMPO_H
#define PROJECT_MODULE_LIBTEMPO_H

#include <pybind11/pybind11.h>
#include <armadillo>
#include <tempogram_processing.h>
#include <tempogram_utils.h>
#include <curve_utils.h>
#include <mat_utils.h>

using namespace arma;
using namespace libtempo;
namespace py = pybind11;

void register_libtempo(pybind11::module &m) {
    m.def("audio_to_novelty_curve",
          [](fmat s, int sr, int window_length, int hop_length, double compression_c, bool log_compression,
             int resample_feature_rate) {
              int feature_rate;
              auto novelty_curve = tempogram_processing::audio_to_novelty_curve
                      (feature_rate, s, sr, window_length, hop_length, compression_c, log_compression,
                       resample_feature_rate);
              return std::make_tuple(novelty_curve, feature_rate);
          },
          R"pbdoc(
          Computes a complex valued fourier tempogram for a given novelty curve

          Args:
              signal: wavefrom of audio signal
              sr: sampling rate of the audio (Hz)
              window_length: window length for STFT (in samples)
              hop_length: stepsize for the STFT
              compression_c: constant for log compression
              log_compression: enable/disable log compression
              resample_feature_rate: feature rate of the resulting novelty curve (resampled, independent of stepsize)

          Returns:
              novelty_curve, feature_rate
          )pbdoc",
          py::arg("signal"),
          py::arg("sr"),
          py::arg("window_length") = -1,
          py::arg("hop_length") = -1,
          py::arg("compression_c") = 1000,
          py::arg("log_compression") = true,
          py::arg("resample_feature_rate") = 200
    );


    m.def("novelty_curve_to_tempogram",
          [](const mat &novelty_curve, const mat &bpm, double feature_rate, int tempo_window, int hop_length) {
              vec t;
              auto tempogram = tempogram_processing::novelty_curve_to_tempogram_dft
                      (t, novelty_curve, bpm, feature_rate, tempo_window, hop_length);
              auto normalized_tempogram = mat_utils::colwise_normalize_p1(tempogram, 2, 0.0001);

              return std::make_tuple(normalized_tempogram, t);
          },
          R"pbdoc(
          Computes a complex valued fourier tempogram for a given novelty curve

          Args:
              novelty_curve: a novelty curve indicating note onset positions
              bpm: vector containing BPM values to compute.
              feature_rate: feature rate of the novelty curve (Hz). This needs to be set to allow for setting other parameters in seconds!
              tempo_window: Analysis window length in seconds
              hop_length: window hop length in frames (of novelty curve)

          Returns:
              tempogram, time vector
          )pbdoc",
          py::arg("novelty_curve"),
          py::arg("bpm"),
          py::arg("feature_rate"),
          py::arg("tempo_window"),
          py::arg("hop_length") = -1
    );

    m.def("tempogram_to_cyclic_tempogram",
          [](const cx_mat &tempogram, const mat &bpm, int octave_divider, int ref_tempo) {
              vec y_axis;
              auto cyclic_tempogram = tempogram_processing::tempogram_to_cyclic_tempogram
                      (y_axis, tempogram, bpm, octave_divider, ref_tempo);

              return std::make_tuple(cyclic_tempogram, y_axis);
          },
          R"pbdoc(
          Computes a cyclic tempogram representation of a tempogram by identifying octave equivalences, simnilar as for chroma features.

          Args:
              tempogram: a tempogram representation
              bpm: tempo axis of the tempogram (in bpm)
              octave_divider: number of tempo classes used for representing a tempo octave. This parameter controls the dimensionality of cyclic tempogram
              ref_tempo: reference tempo defining the partition of BPM into tempo octaves

          Returns:
              cyclic_tempogram, cyclic_axis
          )pbdoc",
          py::arg("tempogram"),
          py::arg("bpm"),
          py::arg("octave_divider") = 120,
          py::arg("ref_tempo") = 60
    );

    m.def("smoothen_tempogram",
          [](const mat &tempogram, const mat &y_axis, const mat &t, double temporal_unit_size, float triplet_weight) {
              int smooth_length_samples = (int) (temporal_unit_size / (t.at(1) - t.at(0)));
              auto smooth_tempogram = tempogram_utils::smoothen_tempogram
                      (tempogram, y_axis, smooth_length_samples, triplet_weight);

              return smooth_tempogram;
          },
          R"pbdoc(
          Smoothens tempogram to prepare it for peak extraction.
          It adds triplets to the intensities, does temporal accumulation, denoising and normalization.

          Args:
              tempogram: a tempogram representation
              y_axis: y axis values of the tempogram
              t: x axis values of the tempogram
              temporal_unit_size: length in seconds over which the tempogram will be stabilized to extract a steady tempo
              triplet_weight: weight of the triplet intensity which will be adeed to its base intensity

          Returns:
              smoothed_tempogram

          )pbdoc",
          py::arg("tempogram"),
          py::arg("y_axis"),
          py::arg("t"),
          py::arg("temporal_unit_size") = 20.,
          py::arg("triplet_weight") = 0.8f
    );

    m.def("tempogram_to_tempo_curve", &tempogram_utils::extract_tempo_curve,
          R"pbdoc(
          Creates curve with values of the bins with max intensities from the tempogram.

          Args:
              tempogram: a tempogram representation
              y_axis: y axis values of the tempogram
    
          Returns:
              tempo_curve
          )pbdoc",
          py::arg("tempogram"),
          py::arg("y_axis")
    );

    m.def("correct_tempo_curve",
          [](const mat &tempo_curve, const mat &t, double min_section_length = 10) {
              int min_section_length_samples = (int) (min_section_length / (t.at(1) - t.at(0)));
              auto corrected_curve = curve_utils::correct_curve_by_length(tempo_curve, min_section_length_samples);
              return corrected_curve;
          },
          R"pbdoc(
          Correct curve by removing short value changes and thus removing small sudden spikes.

          Args:
              tempo_curve
              t: x axis values of the curve
              min_length: maximum section length in seconds after which section is split in half

          Returns:
              tempo_curve
          )pbdoc",
          py::arg("tempo_curve"),
          py::arg("t"),
          py::arg("min_section_length") = 10
    );

    py::class_<libtempo::curve_utils::Section>(m, "Section")
            .def(py::init<double, double, double, double>(), py::arg("start"), py::arg("end"), py::arg("bpm"),
                 py::arg("offset"))
            .def_readwrite("start", &libtempo::curve_utils::Section::start)
            .def_readwrite("end", &libtempo::curve_utils::Section::end)
            .def_readwrite("bpm", &libtempo::curve_utils::Section::bpm)
            .def_readwrite("offset", &libtempo::curve_utils::Section::offset);

    m.def("curve_to_sections", [](const mat &curve, const mat &t, double bpm_reference, double max_section_size,
                                  float bpm_merge_threshold) {
              auto segments = curve_utils::split_curve(curve);
              auto sections_tmp = curve_utils::tempo_segments_to_sections(segments, curve, t, bpm_reference);
              // Merge sections for consistency
              sections_tmp = curve_utils::merge_sections(sections_tmp, bpm_merge_threshold);
              // Split section for precision
              std::vector<curve_utils::Section> sections;
              for (const auto &section : sections_tmp) curve_utils::split_section(section, sections, max_section_size);

              return sections;
          },
          R"pbdoc(
          Converts given curve to sections. Inbetween it clens some inconsistencies to extract as much straight
          lines as possible.

          Args:
              tempo_curve
              t: x axis values of the curve
              bpm_reference: reference bpm for the curve
              max_section_size: maximum section length after which it will be split in half. Use for more accurate offset estimation. Keep value high if you want as few sections as possible
              bpm_merge_threshold: Threshold within which similar bpm will be merged into the same section.

          Returns:
              sections
          )pbdoc",
          py::arg("curve"),
          py::arg("t"),
          py::arg("bpm_reference") = DEFAULT_REF_TEMPO,
          py::arg("max_section_size") = 60,
          py::arg("bpm_merge_threshold") = 0.5f
    );

    m.def("sections_extract_offset",
          [](const mat &novelty_curve, std::vector<curve_utils::Section> sections,
             const std::vector<int> &tempo_multiples, int feature_rate, float bpm_doubt_window, double bpm_doubt_step) {
              for (auto &section : sections) {
                  curve_utils::extract_offset(novelty_curve, section, tempo_multiples, feature_rate, bpm_doubt_window,
                                              bpm_doubt_step);
              }

              return sections;
          },
          R"pbdoc(
          Finds the optimal offset for given section. Also tunes the bpm to a more fitting value

          Args:
              novelty_curve: a novelty curve indicating note onset positions
              sections: sections you want offset to be calculated for.
              tempo_multiples: tempo multiples to consider when searchin for correct offset
              feature_rate: feature rate of the novelty curve (Hz).
              bpm_doubt_window: window around candidate bpm which to search for a more fine and correct bpm
              bpm_doubt_step: steps which to take inside the doubt window to fine tune the bpm

          Returns:
              sections
          )pbdoc",
          py::arg("novelty_curve"),
          py::arg("sections"),
          py::arg("tempo_multiples"),
          py::arg("feature_rate"),
          py::arg("bpm_doubt_window") = 2,
          py::arg("bpm_doubt_step") = 0.1
    );
}

#endif //PROJECT_MODULE_LIBTEMPO_H
