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
typedef py::array_t<float, py::array::f_style | py::array::forcecast> pyarr_f;
typedef py::array_t<arma::cx_double, py::array::f_style | py::array::forcecast> pyarr_cd;

using namespace arma;

/**
 * Converts a python array/matrix type to armadillo type
 * @tparam A Armadollio return type
 * @tparam T Element type
 * @param pyarr
 * @return
 */
template<typename T>
inline Mat<T> py_to_arma_mat(py::array_t<T, py::array::f_style | py::array::forcecast> &pyarr) {
    py::buffer_info info = pyarr.request(); // TODO: Can we move return type to arguments for deduction?
    unsigned long n_rows = info.shape[0];
    unsigned long n_cols = info.shape.size() >= 2 ? info.shape[1] : 1;
    Mat<T> amat(reinterpret_cast<typename Mat<T>::elem_type *>(info.ptr), n_rows, n_cols);
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
    std::vector<size_t> shape = {mat.n_rows, mat.n_cols};
    std::vector<size_t> strides = {sizeof(T), sizeof(T) * mat.n_rows};
    py::buffer_info buffer(
            mat.memptr(),
            sizeof(T),
            py::format_descriptor<T>::format(),
            2,
            shape,
            strides
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

template<typename T>
struct MatrixWrapper {
    Mat<T> data;

    using ArrayType = py::array_t<T, py::array::f_style | py::array::forcecast>;

    explicit MatrixWrapper(Mat<T> data) : data(std::move(data)) {}

    std::tuple<unsigned long, unsigned long> get_shape() { return std::make_tuple(data.n_rows, data.n_cols); }

    ArrayType to_array() {
        return arma_to_py(data);
    }
};

using MatWD = MatrixWrapper<double>;
using MatWF = MatrixWrapper<float>;
using MatWCD = MatrixWrapper<cx_double>;

template<typename T>
inline MatrixWrapper<T> wrap_array(py::array_t<T, py::array::f_style | py::array::forcecast> &array) {
    auto data = py_to_arma_mat<T>(array);
    return MatrixWrapper<T>(data);
}

#endif //TEMPOGRAM_TEMPO_ESTIMATION_PYARMA_HPP
