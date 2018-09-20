#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "libtempo_wrapper.hpp"
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
           normalize_feature
           audio_to_novelty_curve
           audio_to_novelty_curve_tempogram
           tempogram_to_cyclic_tempogram
    )pbdoc";

    m.def("novelty_curve_to_tempogram_dft", &libtempo_wrapper::novelty_curve_to_tempogram_dft,
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
          Tuple[array, array, array]
              tempogram, bpm, time vector
          )pbdoc",
          py::arg("novelty_curve"),
          py::arg("bpm"),
          py::arg("feature_rate"),
          py::arg("tempo_window"),
          py::arg("hop_length") = -1
    );

    m.def("normalize_feature", &libtempo_wrapper::normalize_feature,
          R"pbdoc(
          Normalizes a feature sequence according to the l^p norm
          If the norm falls below threshold for a feature vector, then the normalized feature vector is set to be the
          unit vector.

          Parameters
          ----------
          feature
          p
          threshold

          Returns
          -------
          array
              normalized feature
          )pbdoc",
          py::arg("feature"),
          py::arg("p"),
          py::arg("threshold")
    );

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
          Tuple[array, int]
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

    m.def("audio_to_novelty_curve_tempogram", &libtempo_wrapper::audio_to_novelty_curve_tempogram,
          R"pbdoc(
          Computes a novelty curve and a complex valued fourier tempogram for a given audio signal.

          Parameters
          ----------
          signal
              wavefrom of audio signal
          sr
              sampling rate of the audio (Hz)
          bpm
              vector containing BPM values to compute.
          tempo_window
              Analysis window length in seconds
          hop_length
              window hop length in frames (of novelty curve)

          Returns
          -------
          Tuple[array, int, array, array, array]
              novelty_curve, novelty curve feature rate, tempogram, tempogram frequencies, tempogram times
          )pbdoc",
          py::arg("signal"),
          py::arg("sr"),
          py::arg("bpm"),
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
          Tuple[array, array]
              cyclic_tempogram, cyclic_axis
          )pbdoc",
          py::arg("tempogram"),
          py::arg("bpm"),
          py::arg("octave_divider") = 120,
          py::arg("ref_tempo") = 60
    );

    py::class_<libtempo::curve_utils::Section>(m, "Section")
            .def(py::init<double, double, double, double>())
            .def_readwrite("start", &libtempo::curve_utils::Section::start)
            .def_readwrite("end", &libtempo::curve_utils::Section::end)
            .def_readwrite("bpm", &libtempo::curve_utils::Section::bpm)
            .def_readwrite("offset", &libtempo::curve_utils::Section::offset);

    m.def("smoothen_tempogram", &libtempo_wrapper::smoothen_tempogram,
          R"pbdoc(

          )pbdoc",
          py::arg("tempogram"),
          py::arg("axis_lut"),
          py::arg("temporal_unit_size") = 100,
          py::arg("triplet_weight") = 0.8f
    );

    m.def("tempogram_to_tempo_curve_corrected", &libtempo_wrapper::tempogram_to_tempo_curve_corrected,
          R"pbdoc(

          )pbdoc",
          py::arg("tempogram"),
          py::arg("axis_lut"),
          py::arg("min_length") = 40
    );

    m.def("curve_to_sections", &libtempo_wrapper::curve_to_sections,
          R"pbdoc(

          )pbdoc",
          py::arg("curve"),
          py::arg("t"),
          py::arg("bpm_reference") = DEFAULT_REF_TEMPO,
          py::arg("max_section_size") = 60
    );

    m.def("sections_extract_offset", &libtempo_wrapper::sections_extract_offset,
          R"pbdoc(

          )pbdoc",
          py::arg("novelty_curve"),
          py::arg("sections"),
          py::arg("tempo_multiples"),
          py::arg("feature_rate"),
          py::arg("bpm_doubt_window") = 2,
          py::arg("bpm_doubt_step") = 0.1
    );
};