//
// Created by egordm on 30-8-2018.
//

#include "signal_utils.h"

std::tuple<vec, vec>
libtempo::signal_utils::generate_pulse(double bpm, int window_length, int feature_rate, int shift_by) {
    double freq = bpm / 60.;
    vec T = regspace<vec>(-shift_by, window_length - 1 - shift_by) / feature_rate;
    vec twoPiFt = freq * 2 * M_PI * T;

    return std::make_tuple(cos(twoPiFt), sin(twoPiFt));
}

fvec libtempo::signal_utils::generate_click(int sr, double duration, double freq) {
    double angular_freq = 2 * M_PI * freq / sr;
    fvec click = exp2(linspace<fvec>(0, -10, (uword) round(sr * duration)));
    click %= sin(angular_freq * regspace<fvec>(0, click.n_rows - 1));

    return click;
}

fvec libtempo::signal_utils::generate_click_track(const std::vector<float> &positions, unsigned long length, int sr) {
    fvec ret(static_cast<const uword>(length), fill::zeros);
    fvec click = generate_click(sr);

    for (const auto &pos : positions) {
        auto position = static_cast<uword>(pos * sr);

        if (position > ret.n_rows || position < 0) continue;
        auto click_length = std::min(click.n_rows, ret.n_rows - position);

        ret(span(position, position + click_length - 1)) = click(span(0, click_length - 1));
    }

    return ret;
}

fvec libtempo::signal_utils::generate_click_track(double bpm, double offset, int note_fraction, unsigned long length,
                                                  int sr) {
    std::vector<float> positions;
    float end = length / (float) sr;
    auto position = (float) offset;

    double bar_len = 60. / bpm * 4;
    double fraction_note_len = bar_len / note_fraction;

    while (position < end) {
        positions.push_back(position);
        position += fraction_note_len;
    }

    return generate_click_track(positions, length, sr);
}