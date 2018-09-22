//
// Created by egordm on 15-9-2018.
//

#ifndef PROJECT_FOURIER_UTILS_H
#define PROJECT_FOURIER_UTILS_H

#include <armadillo>
#include <cmath>
#include <sigpack.h>
#include <fftw/fftw.h>
#include "math_utils.hpp"

using namespace arma;
using namespace sp;


namespace libtempo { namespace fourier_utils {

    /**
    * returns the complex fourier coefficients of frequency f of the signal s
    * @param freqs
    * @param t
    * @param s time domain signal
    * @param window vector containing window function
    * @param n_overlap overlap given in samples
    * @param f vector of frequencies values of fourier coefficients, in Hz
    * @param sr sampling rate of signal s in Hz
    * @return complex fourier coefficients
    */
    cx_mat
    compute_fourier_coefficients(vec &t, const vec &s, const vec &window, int n_overlap, const vec &f, double sr);

    /**
    * Computes a spectrogram using a STFT (short-time fourier transform)
    * @param signal: wavefrom of audio signal
    * @param sr: sample rate
    * @param window
    * @param coefficient_range
    * @param n_fft: window length
    * @param hop_length
    * @return spectrogram
    */
    template<typename T>
    mat stft(float &feature_rate, vec &t, vec &f, const Col<T> &signal, int sr, const vec &window,
             std::tuple<int, int> coefficient_range, int n_fft = -1, int hop_length = -1) {
        auto window_length = static_cast<int>(window.size());
        if (hop_length <= 0) hop_length = static_cast<int>(round(window_length / 2.));
        if (n_fft <= 0) n_fft = window_length;

        // Precalculate
        feature_rate = (float) sr / hop_length;
        auto signal_size = static_cast<int>(signal.size());

        auto first_window = (int) floor(window_length / 2.);
        auto num_frames = (int) ceil((double) signal_size / hop_length);
        int num_coeffs = std::get<1>(coefficient_range) - std::get<0>(coefficient_range);
        int frame_pad = std::max(0, n_fft - window_length);

        // Spectrogram alloc
        mat s((const uword) num_coeffs, (const uword) num_frames);

        // first window's center is at 0 seconds
        ivec frame = linspace<ivec>(0, window_length - 1, (const uword) window_length) - first_window;

        // Allocate window chunk
        vec x((uword) window_length);

        sp::FFTW fftw(static_cast<unsigned int>(frame.size())); // TODO: openmp mb
        for (int i = 0; i < num_frames; ++i) {
            x.zeros();

            // Zero pad data to place into the windows
            uword pad_before_n = (uword) sum(frame <= 0);
            uword pad_after_n = (uword) std::max(0, (int) (window_length - (signal.size() - frame(0))));
            x(span(pad_before_n, window_length - pad_after_n - frame_pad - 1)) =
                    conv_to<vec>::from(
                            signal(span((uword) frame(pad_before_n), (uword) frame(frame.size() - pad_after_n - 1))));

            // Apply the window
            x(span(0, static_cast<const uword>(window_length - frame_pad - 1))) %= window;

            // Fast fourier transform
            cx_vec Xs = fftw.fft(x);

            // Convert to magnitude. Not complex anymore
            s(span::all, (const uword) i) = abs(Xs(span((const uword) std::get<0>(coefficient_range),
                                                        (const uword) std::get<1>(coefficient_range) - 1)));

            frame += hop_length; // Move to next frame
        }

        // Calculate the axes
        auto half_window = (int) floor(fmax(n_fft, window_length) / 2.);
        t = regspace<vec>(0, s.n_cols - 1) * (hop_length / (double) sr);
        f = linspace<vec>(0, half_window - 1, (const uword) half_window) / (double) half_window * (sr / 2.);
        f = f(span((const uword) std::get<0>(coefficient_range), (const uword) std::get<1>(coefficient_range) - 2));

        return s;
    }

    template<typename T>
    mat stft(float &feature_rate, vec &t, vec &f, const Col<T> &s, int sr, const vec &window, int n_fft = -1,
             int hop_length = -1) {
        std::tuple<int, int> coefficient_range = std::make_tuple(0, (int) floor(fmax(n_fft, (int) window.size()) / 2.) + 1);
        return fourier_utils::stft(feature_rate, t, f, s, sr, window, coefficient_range, n_fft, hop_length);
    }
}}


#endif //PROJECT_FOURIER_UTILS_H
