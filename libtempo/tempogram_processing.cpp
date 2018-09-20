//
// Created by egordm on 28-7-2018.
//

#include <sigpack.h>
#include "tempogram_processing.h"
#include "fourier_utils.h"
#include "math_utils.hpp"
#include "resample.h"
#include "mat_utils.h"

using namespace sp;
using namespace libtempo;

vec tempogram_processing::audio_to_novelty_curve(int &feature_rate_ret, const fvec &signal, int sr, int window_length,
                                                 int hop_length, double compression_c,
                                                 bool log_compression, int resample_feature_rate) {
    if (window_length <= 0) window_length = static_cast<int>(1024 * sr / 22050.);
    if (hop_length <= 0) hop_length = static_cast<int>(512 * sr / 22050.);

    vec window = utils::math::my_hanning(static_cast<const uword>(window_length));
    float feature_rate;
    vec stft_t;
    vec stft_f;
    auto stft = fourier_utils::stft(feature_rate, stft_t, stft_f, signal, sr, window, window_length, hop_length);

    // normalize and convert to dB
    mat spe = stft / stft.max();
    double thresh = -74;
    thresh = pow(10, thresh / 20);
    spe = clamp(spe, thresh, spe.max());

    // bandwise processing
    mat bands = {{0,      500},
                 {500,    1250},
                 {1250,   3125},
                 {3125,   7812.5},
                 {7812.5, floor(sr / 2.)}};
    mat band_novelty_curves(bands.n_rows, spe.n_cols, fill::zeros);

#pragma omp parallel for
    for (int i = 0; i < bands.n_rows; ++i) { // TODO: Contents rly need to be split in separete funcs. But also not idk
        rowvec bins = round(bands(i, span::all) / (sr / (double) window_length));
        bins = clamp(bins, 0, window_length / 2.);

        // band novelty curve
        mat band_data = spe(span((const uword) bins(0), (const uword) bins(1) - 1), span::all);
        if (log_compression && compression_c > 0)
            band_data = log(1 + band_data * compression_c) / log(1 + compression_c);

        // smoothed differentiator
        double diff_len = 0.3;
        diff_len = max(ceil(diff_len * sr / (double) hop_length), 5.);
        diff_len = 2 * round(diff_len / 2.) + 1;
        auto half_diff_len = (const uword) floor(diff_len / 2);

        vec mult_filt = join_cols(-1 * vec(half_diff_len, fill::ones), vec(1, fill::zeros));
        mult_filt = join_cols(mult_filt, vec(half_diff_len, fill::ones));
        rowvec diff_filter = (utils::math::my_hanning((const uword) diff_len) % mult_filt).t();

        mat band_krn = utils::math::pad_mat(band_data, (int) half_diff_len);
        mat band_diff = conv2(band_krn, flipud(fliplr(diff_filter)), "same");
        band_diff = band_diff % (band_diff > 0);
        band_diff = band_diff(span::all, span(half_diff_len - 1, band_diff.n_cols - half_diff_len - 2));

        // normalize band
        double norm_len = 5; // sec
        norm_len = max(ceil(norm_len * sr / hop_length), 3.);
        rowvec norm_filter = utils::math::my_hanning((const uword) norm_len).t();
        mat norm_curve = conv2(sum(band_data, 0), flipud(fliplr(norm_filter / sum(norm_filter))), "same");

        // boundary correction
        double norm_filter_sum_sub = sum(sum(norm_filter));
        mat norm_filter_sum = (norm_filter_sum_sub - cumsum(norm_filter, 1)) / norm_filter_sum_sub;
        auto half_norm_len = (const uword) (floor(norm_len / 2.));
        auto f_half_span = span(0, half_norm_len - 1);
        auto l_half_span = span(norm_curve.n_cols - half_norm_len, norm_curve.n_cols - 1);
        norm_curve(0, f_half_span) = norm_curve(0, f_half_span) / fliplr(norm_filter_sum(0, f_half_span));
        norm_curve(0, l_half_span) = norm_curve(0, l_half_span) / norm_filter_sum(0, f_half_span);

        for (uword r = 0; r < band_diff.n_rows; ++r) {
            band_diff(r, span::all) /= norm_curve;
        }

        // compute novelty curve of band
        mat novelty_curve = sum(band_diff, 0);
        band_novelty_curves(i, span::all) = novelty_curve;
    }

    vec novelty_curve = mean(band_novelty_curves, 0).t();

    // resample curve
    if (resample_feature_rate > 0 && resample_feature_rate != feature_rate) {
        const int p = (const int) round(1000 * resample_feature_rate / feature_rate);
        const int q = 1000;

        novelty_curve = libtempo::resample(novelty_curve, p, q);
        feature_rate = (float) resample_feature_rate;
    }

    novelty_curve = mat_utils::smooth_filter_subtract(novelty_curve, sr, hop_length);

    feature_rate_ret = feature_rate;
    return novelty_curve;
}


cx_mat tempogram_processing::novelty_curve_to_tempogram_dft(vec &t, const vec &novelty_curve, const vec &bpms,
                                                            double feature_rate, int tempo_window, int hop_length) {
    if (hop_length <= 0) hop_length = (int) ceil(feature_rate / 5.);

    // Create window
    auto win_length = static_cast<int>(round(tempo_window * feature_rate));
    win_length = win_length + (win_length % 2) - 1;

    auto window = sp::hann(static_cast<const uword>(win_length));
    auto half_window = (unsigned int) round(win_length / 2.0);

    vec nc = mat_utils::pad_vec(novelty_curve, half_window, half_window);

    // Compute tempogram
    auto ret = fourier_utils::compute_fourier_coefficients(t, nc, window, win_length - hop_length, bpms / 60.,
                                                           feature_rate);

    // Normalize and return
    t -= t(0);
    return ret / sqrt((double) win_length) / sum(window) * win_length;
}

mat tempogram_processing::tempogram_to_cyclic_tempogram(vec &y_axis, const cx_mat &tempogram, const vec &bpm,
                                                        int octave_divider, int ref_tempo) {
    double ref_octave = ref_tempo / min(bpm);
    int min_octave = static_cast<int>(round(log2(min(bpm) / ref_tempo)));
    int max_octave = static_cast<int>(round(log2(max(bpm) / ref_tempo)) + 1);

    // to real vals
    mat mag_tempogram = abs(tempogram);

    // rescale to log tempo axis tempogram. Each octave is represented by octave_divider tempi
    vec log_bpm = ref_tempo * exp2(regspace<vec>(min_octave, 1. / octave_divider, (max_octave - 1. / octave_divider)));
    mat log_tempogram;
    mat_utils::mat_interp1_nearest(bpm, mag_tempogram, log_bpm, log_tempogram);

    // cyclic projection of log axis tempogram to the reference octave
    mat cyclic_tempogram((const uword) octave_divider, mag_tempogram.n_cols, fill::zeros);
    uword end_pos = ((uvec)find(log_bpm < max(bpm), 1, "last")).at(0);
    for (int i = 0; i < octave_divider; ++i) {
        cyclic_tempogram((uword)i, span::all) =
                mean(log_tempogram.rows(regspace<uvec>((uword)i, (uword)octave_divider,  end_pos)));
    }

    y_axis = ref_octave * log_bpm(span(0, (const uword) octave_divider - 1)) / ref_tempo;
    return cyclic_tempogram;
}