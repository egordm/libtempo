#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "libtempo_wrapper.hpp"
#include "binding_helper.h"
#include <curve_utils.h>

namespace py = pybind11;

PYBIND11_MODULE(libtempo_py, m) {
    m.doc() = R"pbdoc(
        libtempo_py
        -----------------------
        .. currentmodule:: libtempo_py
        .. autosummary::
           :toctree: _generate

           novelty_curve_to_tempogram_dft
           audio_to_novelty_curve
           tempogram_to_cyclic_tempogram
    )pbdoc";

    binding_helper::define_matrix_wrapper<double>(m);
    binding_helper::define_matrix_wrapper<float>(m);
    binding_helper::define_matrix_wrapper<cx_double>(m);

    m.def("audio_to_novelty_curve", &libtempo_wrapper::audio_to_novelty_curve,
          R"pbdoc(
          Computes a complex valued fourier tempogram for a given novelty curve

          Parameters
          ----------
          signal
              wavefrom of audio signal
          sr
              sampling rate of the audio (Hz)
          window_length
              window length for STFT (in samples)
          hop_length
              stepsize for the STFT
          compression_c
              constant for log compression
          log_compression
              enable/disable log compression
          resample_feature_rate
              feature rate of the resulting novelty curve (resampled, independent of stepsize)

          Returns
          -------
          Tuple[MatrixWrapper, int]
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


    m.def("novelty_curve_to_tempogram", &libtempo_wrapper::novelty_curve_to_tempogram,
          R"pbdoc(
          Computes a complex valued fourier tempogram for a given novelty curve

          Parameters
          ----------
          novelty_curve
              a novelty curve indicating note onset positions
          bpm
              vector containing BPM values to compute.
          feature_rate
              feature rate of the novelty curve (Hz). This needs to be set to allow for setting other parameters in seconds!
          tempo_window
              Analysis window length in seconds
          hop_length
              window hop length in frames (of novelty curve)

          Returns
          -------
          Tuple[MatrixWrapper, MatrixWrapper, MatrixWrapper]
              tempogram, bpm, time vector
          )pbdoc",
          py::arg("novelty_curve"),
          py::arg("bpm"),
          py::arg("feature_rate"),
          py::arg("tempo_window"),
          py::arg("hop_length") = -1
    );

    m.def("tempogram_to_cyclic_tempogram", &libtempo_wrapper::tempogram_to_cyclic_tempogram,
          R"pbdoc(
          Computes a cyclic tempogram representation of a tempogram by identifying octave equivalences, simnilar as for chroma features.

          Parameters
          ----------
          tempogram
              a tempogram representation
          bpm
              tempo axis of the tempogram (in bpm)
          octave_divider
              number of tempo classes used for representing a tempo octave. This parameter controls the dimensionality of cyclic tempogram
          ref_tempo
              reference tempo defining the partition of BPM into tempo octaves

          Returns
          -------
          Tuple[MatrixWrapper, MatrixWrapper]
              cyclic_tempogram, cyclic_axis
          )pbdoc",
          py::arg("tempogram"),
          py::arg("bpm"),
          py::arg("octave_divider") = 120,
          py::arg("ref_tempo") = 60
    );

    m.def("smoothen_tempogram", &libtempo_wrapper::smoothen_tempogram,
          R"pbdoc(
          Smoothens tempogram to prepare it for peak extraction.
          It adds triplets to the intensities, does temporal accumulation, denoising and normalization.

          Parameters
          ----------
          tempogram
              a tempogram representation
          y_axis
              y axis values of the tempogram
          t
              x axis values of the tempogram
          temporal_unit_size
              length over which the tempogram will be stabilized to extract a steady tempo
          triplet_weight
              weight of the triplet intensity which will be adeed to its base intensity

          Returns
          -------
          MatrixWrapper
              smoothed_tempogram

          )pbdoc",
          py::arg("tempogram"),
          py::arg("y_axis"),
          py::arg("t"),
          py::arg("temporal_unit_size") = 20.,
          py::arg("triplet_weight") = 0.8f
    );

    m.def("tempogram_to_tempo_curve", &libtempo_wrapper::tempogram_to_tempo_curve,
          R"pbdoc(
          Creates curve with values of the bins with max intensities from the tempogram.

          Parameters
          ----------
          tempogram
              a tempogram representation
          y_axis
              y axis values of the tempogram

          Returns
          -------
          MatrixWrapper
              tempo_curve
          )pbdoc",
          py::arg("tempogram"),
          py::arg("y_axis")
    );

    m.def("correct_tempo_curve", &libtempo_wrapper::correct_tempo_curve,
          R"pbdoc(
          Correct curve by removing short value changes and thus removing small sudden spikes.

          Parameters
          ----------
          tempo_curve
          t
              x axis values of the curve
          min_length
              maximum section length in seconds after which section is split in half

          Returns
          -------
          MatrixWrapper
              tempo_curve
          )pbdoc",
          py::arg("tempo_curve"),
          py::arg("t"),
          py::arg("min_section_length") = 10
    );

    py::class_<libtempo::curve_utils::Section>(m, "Section")
            .def(py::init<double, double, double, double>())
            .def_readwrite("start", &libtempo::curve_utils::Section::start)
            .def_readwrite("end", &libtempo::curve_utils::Section::end)
            .def_readwrite("bpm", &libtempo::curve_utils::Section::bpm)
            .def_readwrite("offset", &libtempo::curve_utils::Section::offset);

    m.def("curve_to_sections", &libtempo_wrapper::curve_to_sections,
          R"pbdoc(
          Converts given curve to sections. Inbetween it clens some inconsistencies to extract as much straight
          lines as possible.

          Parameters
          ----------
          tempo_curve
          t
              x axis values of the curve
          bpm_reference
              reference bpm for the curve
          max_section_size
              maximum section length after which it will be split in half. Use for more accurate offset estimation.
              Keep value high if you want as few sections as possible
          bpm_merge_threshold
              Threshold within which similar bpm will be merged into the same section.

          Returns
          -------
          List[Section]
              sections
          )pbdoc",
          py::arg("curve"),
          py::arg("t"),
          py::arg("bpm_reference") = DEFAULT_REF_TEMPO,
          py::arg("max_section_size") = 60,
          py::arg("bpm_merge_threshold") = 0.5f
    );

    m.def("sections_extract_offset", &libtempo_wrapper::sections_extract_offset,
          R"pbdoc(
          Finds the optimal offset for given section. Also tunes the bpm to a more fitting value

          Parameters
          ----------
          novelty_curve
              a novelty curve indicating note onset positions
          sections
              sections you want offset to be calculated for.
          tempo_multiples
              tempo multiples to consider when searchin for correct offset
          feature_rate
              feature rate of the novelty curve (Hz).
          bpm_doubt_window
               window around candidate bpm which to search for a more fine and correct bpm
          bpm_doubt_step
               steps which to take inside the doubt window to fine tune the bpm

          Returns
          -------
          List[Section]
              sections
          )pbdoc",
          py::arg("novelty_curve"),
          py::arg("sections"),
          py::arg("tempo_multiples"),
          py::arg("feature_rate"),
          py::arg("bpm_doubt_window") = 2,
          py::arg("bpm_doubt_step") = 0.1
    );
};