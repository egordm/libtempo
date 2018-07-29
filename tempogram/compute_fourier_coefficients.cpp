//
// Created by egordm on 26-7-2018.
//

#define _USE_MATH_DEFINES

#include <sigpack.h>
#include "compute_fourier_coefficients.h"
#include "math_utils.hpp"

using namespace tempogram;
using namespace arma;
using namespace sp;


std::tuple<cx_mat, vec, vec>
tempogram::compute_fourier_coefficients(const vec &s, const vec &window, int n_overlap, const vec &f, double sr) {
    int win_length = (int) window.size();
    double win_length_half = win_length / 2.;
    int hop_length = win_length - n_overlap;

    vec T = linspace<vec>(0, win_length - 1, static_cast<const uword>(win_length)) / sr;
    int win_num = utils::math::fix((s.size() - n_overlap) / (win_length - n_overlap));
    cx_mat x(static_cast<const uword>(win_num), f.size(), fill::zeros);
    vec t = linspace(win_length_half, win_length_half + (win_num - 1) * hop_length, (const uword) win_num) / sr;

    vec twoPiT = 2 * M_PI * T;

    int test = 0;

    for (int f0 = 0; f0 < f.size(); ++f0) {
        vec twoPiFt = f[f0] * twoPiT;
        vec cosine = cos(twoPiFt);
        vec sine = sin(twoPiFt);

        for (int w = 0; w < win_num; ++w) {
            int start = w * hop_length;
            int stop = start + win_length;

            vec sig = s.subvec((const uword) start, (const uword) (stop - 1)) % window;
            auto co = sum(sig % cosine);
            auto si = sum(sig % sine);

            x((const uword) w, (const uword) f0) = cx_double(co, si);
        }
    }

    return std::make_tuple(x.st(), f, t);
}

cx_mat tempogram::normalize_feature(const cx_mat &feature, unsigned int p, double threshold) {
    cx_mat ret(feature.n_rows, feature.n_cols, fill::zeros);

    // normalise the vectors according to the l^p norm
    cx_mat unit_vec(feature.n_rows, 1);
    unit_vec.ones();
    unit_vec = unit_vec / norm(unit_vec, p);

    for (int k = 0; k < feature.n_cols; ++k) {
        double n = norm(feature(span::all, (const uword) k), p);

        if (n < threshold) ret(span::all, (const uword) k) = unit_vec;
        else ret(span::all, (const uword) k) = feature(span::all, (const uword) k) / n;
    }

    return ret;
}

tuple<mat, float, vec, vec> tempogram::stft(const vec &s, int sr, const vec &window, int n_fft, int hop_length) {
    std::tuple<int, int> coefficient_range = make_tuple(0, (int) floor(max(n_fft, (int) window.size()) / 2.) + 1);
    return stft(s, sr, window, coefficient_range, n_fft, hop_length);
}

tuple<mat, float, vec, vec>
tempogram::stft(const vec &signal, int sr, const vec &window, std::tuple<int, int> coefficient_range, int n_fft,
                int hop_length) {
    auto window_length = static_cast<int>(window.size());
    if (hop_length <= 0) hop_length = static_cast<int>(round(window_length / 2.));
    if (n_fft <= 0) n_fft = window_length;

    // Precalculate
    float feature_rate = (float) sr / hop_length;
    auto signal_size = static_cast<int>(signal.size());

    auto first_window = (int) floor(window_length / 2.);
    auto num_frames = (int) ceil((double) signal_size / hop_length);
    int num_coeffs = std::get<1>(coefficient_range) - std::get<0>(coefficient_range);
    int n_zero_pad = max(0, n_fft - window_length);

    // Spectrogram calculation
    mat s((const uword) num_coeffs, (const uword) num_frames, fill::zeros);

    // first window's center is at 0 seconds
    ivec frame = linspace<ivec>(0, window_length - 1, (const uword) window_length) - first_window;

    sp::FFTW fftw(static_cast<unsigned int>(frame.size()));
    for (int i = 0; i < num_frames; ++i) {
        int n_zeros = static_cast<int>(sum(frame <= 0));
        int n_vals = static_cast<int>(frame.size() - n_zeros);

        vec x(frame.size());
        if (n_zeros > 0)
            x = join_cols(vec((const uword) n_zeros, fill::zeros), signal(span(0, (const uword) n_vals - 1)));
        else if (frame(frame.size() - 1) >= signal_size)
            x = join_cols(signal(span((const uword) frame(0), signal.size() - 1)),
                          vec(window_length - (signal.size() - frame(0)), fill::zeros));
        else x = signal(span((const uword) frame(0), (const uword) frame(frame.size() - 1)));

        x %= window;

        if (n_zero_pad > 0) x = join_cols(x, vec((const uword) n_zero_pad, fill::zeros));

        cx_vec Xs = fftw.fft(x);
        // Convert to magnitude. Not complex anymore
        s(span::all, (const uword) i) = abs(Xs(span((const uword) std::get<0>(coefficient_range),
                                                    (const uword) std::get<1>(coefficient_range) - 1)));

        frame += hop_length;
    }

    vec t = linspace<vec>(0, s.n_cols - 1, s.n_cols) * hop_length / (double) sr;
    auto zet = (int) floor(max(n_fft, window_length) / 2.);
    vec f = linspace<vec>(0, zet - 1, (const uword) zet) / (double) zet * (sr / 2.);
    f = f(span((const uword) std::get<0>(coefficient_range), (const uword) std::get<1>(coefficient_range) - 2));

    return make_tuple(s, feature_rate, t, f);
}

vec
tempogram::audio_to_novelty_curve(const vec &signal, int sr, int window_length, int hop_length, double compression_c,
                                  bool log_compression, int resample_feature_rate) {
    if (window_length <= 0) window_length = static_cast<int>(1024 * sr / 22050.);
    if (hop_length <= 0) hop_length = static_cast<int>(512 * sr / 22050.);

    vec window = sp::hanning(static_cast<const uword>(window_length));
    auto stft_ret = stft(signal, sr, window, window_length, hop_length);

    // normalize and convert to dB
    mat spe = std::get<0>(stft_ret) / std::get<0>(stft_ret).max();
    double thresh = -74;
    thresh = pow(10, thresh / 20);
    spe = clamp(spe, thresh, spe.max());
    std::cout << spe(0, span(88, 101)) << std::endl;

    // bandwise processing
    mat bands = {{0,      500},
                 {500,    1250},
                 {1250,   3125},
                 {3125,   7812.5},
                 {7812.5, floor(sr / 2.)}};
    mat band_novelty_curve(bands.n_rows, spe.n_cols, fill::zeros);

    for (int i = 0; i < bands.n_rows; ++i) {
        vec band = bands(static_cast<const uword>(i), span::all);
        vec bins = round(band / (sr / (double) window_length));
        bins = clamp(bins, 0, window_length / 2.);

        // band novelty curve
        mat band_data = spe(span((const uword) bins(0), (const uword) bins(1)), span::all);
        if (log_compression && compression_c > 0)
            band_data = log(1 + band_data * compression_c) / log(1 + compression_c);

        // smoothed differentiator
        double diff_len = 0.3;
        diff_len = max(ceil(diff_len * sr / (double) hop_length), 5.);
        diff_len = 2 * round(diff_len / 2.) + 1;

        vec mult_filt = join_cols(-1 * vec((const uword) floor(diff_len / 2), fill::ones), vec(1, fill::zeros));
        mult_filt = join_cols(mult_filt, vec((const uword) floor(diff_len / 2), fill::ones));
        vec diff_filter_vec = sp::hanning((const uword) diff_len) % mult_filt;
        mat diff_filter(1, diff_filter_vec.size());
        diff_filter(0, span::all) = diff_filter_vec;

        //mat band_krn = join_cols()

    }

    return arma::vec();
}