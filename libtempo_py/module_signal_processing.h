//
// Created by egordm on 21-9-2018.
//

#ifndef PROJECT_MODULE_SIGNAL_PROCESSING_H
#define PROJECT_MODULE_SIGNAL_PROCESSING_H

#include <pybind11/pybind11.h>
#include "wrapper_signal_processing.h"

void register_signal_processing(pybind11::module &m) {
    m.def("stft", &wrapper_signal_processing::stft,
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

    m.def("stft_noc", &wrapper_signal_processing::stft_noc,
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

    m.def("compute_fourier_coefficients", &wrapper_signal_processing::compute_fourier_coefficients,
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
