//
// Created by egordm on 30-8-2018.
//

#include "signal_utils.h"

std::tuple<vec, vec> tempogram::signal_utils::generate_pulse(double bpm, int window_length, int feature_rate) {
    double freq = bpm / 60.;
    vec T = regspace<vec>(0, window_length - 1) / feature_rate;
    vec twoPiFt = freq * 2 * M_PI * T;

    return std::make_tuple(cos(twoPiFt), sin(twoPiFt));
}

vec tempogram::signal_utils::generate_click(int sr, double duration, double freq) {
    double angular_freq = 2 * M_PI * freq / sr;
    vec click = exp2(linspace<vec>(0, -10, (uword) round(sr * duration)));
    click %= sin(angular_freq * regspace<vec>(0, click.n_rows - 1));

    return click;
}

vec tempogram::signal_utils::generate_click_track(const std::vector<float> &positions, int length, int sr) {
    vec ret(static_cast<const uword>(length));
    vec click = generate_click(sr);

    for(const auto &pos : positions) {
        auto position = static_cast<uword>(pos * sr);

        if(position > ret.n_rows || position < 0) continue;
        auto click_length = std::min(click.n_rows, ret.n_rows - position);

        ret(span(position, position + click_length - 1)) = click(span(0, click_length - 1));
    }

    return ret;
}
