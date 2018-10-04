#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <math_utils.hpp>
#include <signal_utils.h>
#include <fourier_utils.h>
#include "test_helpers.h"

using namespace libtempo;

class FourierUtilsTests : public ::testing::Test {

};

TEST_F(FourierUtilsTests, SignalUtils_GeneratePulse_Test) {
    // 6000 bpm is 10 ms per beat
    double bpm = 600; // 0.1 sec per beat = 10 beats per sec
    int feature_rate = 20; // 10 samples per second
    auto vals = signal_utils::generate_pulse(bpm, feature_rate, feature_rate, 5);
    auto cos = std::get<0>(vals);
    auto sin = std::get<1>(vals);

    double prev = 1;
    for (int i = 0; i < sin.n_rows; ++i) {
        ASSERT_EQ(cos[i], -prev);
        prev = cos[i];
    }
}

TEST_F(FourierUtilsTests, FourierUtilsTests_ComputeFourierCoefficients_Test) {
    double bpm = 600; // 0.1 sec per beat = 10 beats per sec
    int feature_rate = 10; // samples per second
    auto signal_tpl = signal_utils::generate_pulse(bpm, feature_rate * 10, feature_rate);
    auto signal = std::get<0>(signal_tpl);

    vec window(feature_rate, fill::ones);

    vec t;
    vec f = {150, 300, 600};
    f /= 60;
    mat vals = abs(fourier_utils::compute_fourier_coefficients(t, signal, window, feature_rate / 2, f, feature_rate));

    ASSERT_DOUBLE_EQ(mean(vals(2, span::all)), bpm / 60);
    ASSERT_DOUBLE_EQ(mean(vals(0, span::all)), sqrt(2));

    int half_window = window.n_rows / 2;
    int hoplen = (window.n_rows - feature_rate / 2);
    vec tcomp = linspace(half_window, half_window + (vals.n_cols - 1) * hoplen, (const uword) vals.n_cols)
                / feature_rate;

    ASSERT_TRUE(compare_vec(t, tcomp));
}

TEST_F(FourierUtilsTests, FourierUtilsTests_STFT_Test) {
    double bpm = 600; // 0.1 sec per beat = 10 beats per sec
    int freq = bpm / 60;
    int feature_rate = 20; // samples per second

    auto signal_tpl = signal_utils::generate_pulse(bpm, feature_rate * 10, feature_rate);
    auto signal = std::get<0>(signal_tpl);

    vec window(feature_rate, fill::ones);
    int half_window = window.n_rows / 2;

    vec t;
    vec f;
    float fr;
    auto vals = fourier_utils::stft(fr, t, f, signal, feature_rate, window);

    vec tcomp =regspace<vec>(0, vals.n_cols - 1) * (half_window / (double) feature_rate);

    ASSERT_DOUBLE_EQ(mean(vals(freq, span(1, vals.n_cols - 1))), 2 * bpm / 60);
    ASSERT_TRUE(compare_vec(f, regspace(0, feature_rate/2)));
    ASSERT_TRUE(compare_vec(t, tcomp));
}