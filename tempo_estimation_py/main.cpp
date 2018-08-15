#include <pybind11/pybind11.h>
#include "tempogram_wrapper.hpp"

namespace py = pybind11;

PYBIND11_MODULE(tempo_estimation_py, m) {
    m.doc() = R"pbdoc(
        tempo_estimation_py
        -----------------------
        .. currentmodule:: tempo_estimation_py
        .. autosummary::
           :toctree: _generate

           novelty_curve_to_tempogram_dft
           normalize_feature
           audio_to_novelty_curve
           audio_to_novelty_curve_tempogram
           tempogram_to_cyclic_tempogram
    )pbdoc";

    m.def("novelty_curve_to_tempogram_dft", &tempogram_wrapper::novelty_curve_to_tempogram_dft,
          R"pbdoc(
          Computes a complex valued fourier tempogram for a given novelty curve
          )pbdoc",
          py::arg("novelty_curve_np"),
          py::arg("bpm"),
          py::arg("feature_rate"),
          py::arg("tempo_window"),
          py::arg("hop_length") = -1
    );

    m.def("normalize_feature", &tempogram_wrapper::normalize_feature,
          R"pbdoc(
          Normalizes a feature sequence according to the l^p norm
          )pbdoc",
          py::arg("feature"),
          py::arg("p"),
          py::arg("threshold")
    );

    m.def("audio_to_novelty_curve", &tempogram_wrapper::audio_to_novelty_curve,
          R"pbdoc(
          Computes a complex valued fourier tempogram for a given novelty curve
          )pbdoc",
          py::arg("signal"),
          py::arg("sr"),
          py::arg("window_length") = -1,
          py::arg("hop_length") = -1,
          py::arg("compression_c") = 1000,
          py::arg("log_compression") = true,
          py::arg("resample_feature_rate") = 200
    );

    m.def("audio_to_novelty_curve_tempogram", &tempogram_wrapper::audio_to_novelty_curve_tempogram,
          R"pbdoc(
          Computes a novelty curve and a complex valued fourier tempogram for a given audio signal.
          )pbdoc",
          py::arg("signal"),
          py::arg("sr"),
          py::arg("bpm"),
          py::arg("tempo_window"),
          py::arg("hop_length") = -1
    );

    m.def("tempogram_to_cyclic_tempogram", &tempogram_wrapper::tempogram_to_cyclic_tempogram,
          R"pbdoc(
          Computes a cyclic tempogram representation of a tempogram by identifying octave equivalences, simnilar as for chroma features.
          )pbdoc",
          py::arg("tempogram"),
          py::arg("bpm"),
          py::arg("octave_divider") = 120,
          py::arg("ref_tempo") = 60
    );
};