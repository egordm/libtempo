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

inline arma::mat py_to_mat(pyarr_d &pmat) {
    py::buffer_info info = pmat.request();
    arma::mat amat = arma::mat(reinterpret_cast<arma::mat::elem_type *>(info.ptr), info.shape[0], info.shape[1]);
    return amat;
}

inline arma::vec py_to_vec(pyarr_d &pmat) {
    py::buffer_info info = pmat.request();
    arma::vec avec = arma::vec(reinterpret_cast<arma::mat::elem_type *>(info.ptr), info.shape[0]);
    return avec;
}

inline py::array_t<double> mat_to_py(arma::mat &mat) {
    py::buffer_info buffer(
            mat.memptr(),
            sizeof(double),
            py::format_descriptor<double>::format(),
            2,
            {mat.n_rows, mat.n_cols},
            {sizeof(double), sizeof(double) * mat.n_rows}
    );
    return py::array_t<double>(buffer);
}

inline py::array_t<arma::cx_double> cx_mat_to_py(arma::cx_mat &mat) {
    py::buffer_info buffer(
            mat.memptr(),
            sizeof(arma::cx_double),
            py::format_descriptor<arma::cx_double>::format(),
            2,
            {mat.n_rows, mat.n_cols},
            {sizeof(arma::cx_double), sizeof(arma::cx_double) * mat.n_rows}
    );
    return py::array_t<arma::cx_double>(buffer);
}

inline py::array_t<double> vec_to_py(arma::vec &vec) {
    py::buffer_info buffer(
            vec.memptr(),
            sizeof(double),
            py::format_descriptor<double>::format(),
            1,
            {vec.n_elem},
            {sizeof(double)}
    );
    return py::array_t<double>(buffer);
}

inline py::array_t<arma::sword> uvec_to_py(arma::uvec &vec) {
    py::buffer_info buffer(
            vec.memptr(),
            sizeof(arma::sword),
            py::format_descriptor<arma::sword>::format(),
            1,
            {vec.n_elem},
            {sizeof(arma::sword)}
    );
    return py::array_t<arma::sword>(buffer);
}

#endif //TEMPOGRAM_TEMPO_ESTIMATION_PYARMA_HPP
