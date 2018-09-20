//
// Created by egordm on 20-9-2018.
//

#ifndef PROJECT_BINDING_HELPER_H
#define PROJECT_BINDING_HELPER_H

#include <pybind11/pybind11.h>

namespace binding_helper {
    template<typename T>
    inline void define_matrix_wrapper(pybind11::module &m) {
        py::class_<libtempo_wrapper::MatrixWrapper<T >>(m, "MatrixWrapper")
                .def("get_size", &libtempo_wrapper::MatrixWrapper<T>::get_size)
                .def("to_array", &libtempo_wrapper::MatrixWrapper<T>::to_array);

        m.def("wrap_array", &libtempo_wrapper::wrap_array<T>,
                R"pbdoc(
                Wraps a python array into an own object to speedup the tempo actions.

                Parameters
                ----------
                data
                    A 1 or 2 dimenstional numpy array

                 Returns
                 -------
                 MatrixWrapper
                    wrapper
          )pbdoc", py::arg("data"));
    }
}

#endif //PROJECT_BINDING_HELPER_H
