//
// Created by egordm on 25-7-2018.
//

#ifndef TEMPOGRAM_TEMPO_ESTIMATION_PYARMA_HPP
#define TEMPOGRAM_TEMPO_ESTIMATION_PYARMA_HPP

#include <armadillo>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py=pybind11;

typedef py::array_t<double, py::array::f_style | py::array::forcecast> pyarr_d;
typedef py::array_t<arma::cx_double, py::array::f_style | py::array::forcecast> pyarr_cd;

/**
 * Converts a python array/matrix type to armadillo type
 * @tparam A Armadollio return type
 * @tparam T Element type
 * @param pyarr
 * @return
 */
template<typename A, typename T>
inline A py_to_arma(py::array_t<T, py::array::f_style | py::array::forcecast> &pyarr) {
    py::buffer_info info = pyarr.request(); // TODO: Can we move return type to arguments for deduction?
    A amat(reinterpret_cast<A::elem_type *>(info.ptr), info.shape[0], info.shape[1]);
    return amat;
}

/**
 * Converts an armadillo type to python matrix type
 * @tparam T Element type
 * @param mat
 * @return
 */
template<typename T>
inline py::array_t<T> arma_to_py(arma::Mat<T> &mat) {
    py::buffer_info buffer(
            mat.memptr(),
            sizeof(T),
            py::format_descriptor<T>::format(),
            2,
            {mat.n_rows, mat.n_cols},
            {sizeof(T), sizeof(T) * mat.n_rows}
    );
    return py::array_t<T>(buffer);
}

/**
 * Converts an armadillo type to python array type
 * @tparam T Element type
 * @param mat
 * @return
 */
template<typename T>
inline py::array_t<T> arma_to_py(arma::Col<T> &vec) {
    py::buffer_info buffer(
            vec.memptr(),
            sizeof(T),
            py::format_descriptor<T>::format(),
            1,
            {vec.n_elem},
            {sizeof(T)}
    );
    return py::array_t<T>(buffer);
}

#endif //TEMPOGRAM_TEMPO_ESTIMATION_PYARMA_HPP
