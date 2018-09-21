//
// Created by egordm on 20-9-2018.
//

#ifndef PROJECT_BINDING_HELPER_H
#define PROJECT_BINDING_HELPER_H

#include <pybind11/pybind11.h>

namespace binding_helper {
    template<typename T>
    inline void define_matrix_wrapper(pybind11::module &m, const std::string &type_string) {
        std::string pyclass_name = "MatrixWrapper" + type_string;
        py::class_<wrapper_libtempo::MatrixWrapper<T>, shared_ptr < wrapper_libtempo::MatrixWrapper<T>> >
        (m, pyclass_name.c_str())
                .def("get_size", &wrapper_libtempo::MatrixWrapper<T>::get_size, R"pbdoc(Get data shape)pbdoc")
                .def("to_array", &wrapper_libtempo::MatrixWrapper<T>::to_array, R"pbdoc(Get data as numpy array)pbdoc");

        std::string pyfunc_name = "wrap_array" + type_string;
        m.def(pyfunc_name.c_str(), &wrapper_libtempo::wrap_array<T>,
              R"pbdoc(
                Wraps a python array into an own object to speedup the tempo actions.

                Parameters
                ----------
                data
                    A 1 or 2 dimenstional numpy array

                 Returns
                 -------
                 MatrixWrapperX
                    wrapper
          )pbdoc", py::arg("data"));
    }
}

#endif //PROJECT_BINDING_HELPER_H
