//
// Created by egordm on 1-10-2018.
//

#include "audio_annotation.h"

using namespace libtempo::audio;

fvec annotation::generate_click(int sr, double duration, double freq) {
    double angular_freq = 2 * M_PI * freq / sr;
    fvec click = exp2(linspace<fvec>(0, -10, (uword) round(sr * duration)));
    click %= sin(angular_freq * regspace<fvec>(0, click.n_rows - 1));

    return click;
}

fvec annotation::generate_click_track(const std::vector<float> &positions, unsigned long length, int sr) {
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

fvec annotation::generate_click_track(double bpm, double offset, unsigned long length, int note_fraction, int sr) {
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
