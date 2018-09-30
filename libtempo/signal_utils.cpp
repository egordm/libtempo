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