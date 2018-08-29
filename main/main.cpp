#include <iostream>
#include <audio_processing.h>
#include <tempo_estimation.h>
#include <tempogram_utils.h>
#include <curve_utils.h>
#include <generic_algothms.h>

using namespace std::chrono;
using namespace tempogram;

int main() {
    auto audio = tempogram::audio::open_audio("../../roze.wav");
    mat reduced_sig = mean(audio.data, 0);
    vec signal = reduced_sig.row(0).t();

    auto nov_cv = tempogram::audio_to_novelty_curve(signal, audio.sr);

    vec bpm = regspace(30, 600);
    auto ret = tempogram::novelty_curve_to_tempogram_dft(std::get<0>(nov_cv), bpm, std::get<1>(nov_cv), 8);

    auto normalized_tempogram = tempogram::normalize_feature(std::get<0>(ret), 2, 0.0001);
    auto cyclic_tempgram = tempogram::tempogram_to_cyclic_tempogram(normalized_tempogram, std::get<1>(ret), 120);

    auto smooth_tempogram = tempogram_utils::smoothen_tempogram(std::get<0>(cyclic_tempgram), std::get<1>(cyclic_tempgram));
    auto tempo_curve = tempogram_utils::extract_tempo_curve(smooth_tempogram, std::get<1>(cyclic_tempgram));
    auto tempo_curve_corrected = curve_utils::correct_curve_by_length(tempo_curve, 40);

    return 0;
}