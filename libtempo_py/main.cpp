#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_libtempo.hpp"
#include "binding_helper.h"
#include "module_libtempo.h"
#include "module_audio_io.h"
#include <curve_utils.h>

namespace py = pybind11;

PYBIND11_MODULE(libtempo_py, m) {
    m.doc() = R"pbdoc(
        libtempo_py
        -----------------------
        .. currentmodule:: libtempo_py
        .. autosummary::
           :toctree: _generate

           audio_to_novelty_curve
           novelty_curve_to_tempogram
           tempogram_to_cyclic_tempogram
           smoothen_tempogram
           tempogram_to_tempo_curve
           correct_tempo_curve
           curve_to_sections
           sections_extract_offset
           Section
           MatrixWrapper
    )pbdoc";

    binding_helper::define_matrix_wrapper<double>(m);
    binding_helper::define_matrix_wrapper<float>(m);
    binding_helper::define_matrix_wrapper<cx_double>(m);

    register_libtempo(m);

    auto audio_module = m.def_submodule("audio");
    register_audio_io(audio_module);
};