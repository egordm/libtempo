//
// Created by egordm on 21-9-2018.
//

#ifndef PROJECT_MODULE_SIGNAL_PROCESSING_H
#define PROJECT_MODULE_SIGNAL_PROCESSING_H

#include <pybind11/pybind11.h>
#include <armadillo>
#include <fourier_utils.h>

using namespace arma;
using namespace libtempo;
namespace py = pybind11;

void register_signal_processing(pybind11::module &m) {
    m.def("stft",
          [](const mat &signal, int sr, const mat &window, const std::tuple<int, int> &coefficient_range, int n_fft,
             int hop_length) {
              float feature_rate = 0;
              vec t, f;
              auto stft = fourier_utils::stft<double>(feature_rate, t, f, signal, sr, window, coefficient_range,
                                                      n_fft, hop_length);
              return std::make_tuple(stft, t, f, feature_rate);
          },
          R"pbdoc(
          Computes a spectrogram using a STFT (short-time fourier transform)

          Args:
              signal: wavefrom of audio signal
              sr: sample rate.
              window: vector containing window function
              coefficient_range: range of coefficients to analyze
              n_fft: window length
              hop_length: window hop length in frames

          Returns:
              Tuple[MatrixWrapper, MatrixWrapper, MatrixWrapper]

              spectrogram, frequency vector, time vector
          )pbdoc",
          py::arg("signal"),
          py::arg("sr"),
          py::arg("window"),
          py::arg("coefficient_range"),
          py::arg("n_fft") = -1,
          py::arg("hop_length") = -1
    );

    m.def("stft_noc",
          [](const mat &signal, int sr, const mat &window, int n_fft, int hop_length) {
              float feature_rate = 0;
              vec t, f;
              auto stft = fourier_utils::stft<double>(feature_rate, t, f, signal, sr, window, n_fft, hop_length);
              return std::make_tuple(stft, t, f, feature_rate);
          },
          R"pbdoc(
         Computes a spectrogram using a STFT (short-time fourier transform)

          Args:
              signal: wavefrom of audio signal
              sr: sample rate.
              window: vector containing window function
              n_fft: window length
              hop_length: window hop length in frames

          Returns:
              Tuple[MatrixWrapper, MatrixWrapper, MatrixWrapper]

              spectrogram, frequency vector, time vector
          )pbdoc",
          py::arg("signal"),
          py::arg("sr"),
          py::arg("window"),
          py::arg("n_fft") = -1,
          py::arg("hop_length") = -1
    );

    m.def("compute_fourier_coefficients",
          [](const mat &s, const mat &window, int n_overlap, const mat &f, double sr) {
              vec t;
              auto ret = fourier_utils::compute_fourier_coefficients(t, s, window, n_overlap, f, sr);
              return std::make_tuple(ret, t);
          },
          R"pbdoc(
          returns the complex fourier coefficients of frequency f of the signal s

          Args:
              s: time domain signal
              window: vector containing window function
              n_overlap: overlap given in samples
              f: vector of frequencies values of fourier coefficients, in Hz
              sr: sample rate.

          Returns:
              Tuple[MatrixWrapper, MatrixWrapper]
              complex fourier coefficients, time vector
          )pbdoc",
          py::arg("s"),
          py::arg("window"),
          py::arg("n_overlap"),
          py::arg("f"),
          py::arg("sr")
    );
}

#endif //PROJECT_MODULE_SIGNAL_PROCESSING_H
