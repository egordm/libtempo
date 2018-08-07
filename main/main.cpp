#include <iostream>
#include <audio_processing.h>
#include <tempo_estimation.h>

using namespace std::chrono;

int main() {
    auto audio = tempogram::audio::open_audio("../../valley_of.wav");
    mat reduced_sig = mean(audio.data, 0);
    vec signal = reduced_sig.row(0).t();

    auto nov_cv = tempogram::audio_to_novelty_curve(signal, audio.sr);

    vec bpm = regspace(30, 600);
    auto ret = tempogram::novelty_curve_to_tempogram_dft(std::get<0>(nov_cv), bpm, std::get<1>(nov_cv), 8);

    auto normalized_tempogram = tempogram::normalize_feature(std::get<0>(ret), 2, 0.0001);

    auto cyclic_tempgram = tempogram::tempogram_to_cyclic_tempogram(normalized_tempogram, std::get<1>(ret), 120);

    return 0;
}