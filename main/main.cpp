#include <iostream>
#include <audio_processing.h>
#include <tempogram_processing.h>
#include <tempogram_utils.h>
#include <curve_utils.h>
#include <generic_algothms.h>
#include <defines.h>

using namespace std::chrono;
using namespace tempogram;

int main() {
    auto audio = tempogram::audio::open_audio("../../roze.wav");
    mat reduced_sig = mean(audio.data, 0);
    vec signal = reduced_sig.row(0).t();

    auto nov_cv = tempogram_processing::audio_to_novelty_curve(signal, audio.sr);

    vec bpm = regspace(30, 600);
    auto tempogram_tpl = tempogram_processing::novelty_curve_to_tempogram_dft(std::get<0>(nov_cv), bpm, std::get<1>(nov_cv), 8);

    auto normalized_tempogram = tempogram::normalize_feature(std::get<0>(tempogram_tpl), 2, 0.0001);
    auto cyclic_tempgram = tempogram_processing::tempogram_to_cyclic_tempogram(normalized_tempogram, std::get<1>(tempogram_tpl), 120);

    auto smooth_tempogram = tempogram_utils::smoothen_tempogram(std::get<0>(cyclic_tempgram), std::get<1>(cyclic_tempgram));
    auto tempo_curve = tempogram_utils::extract_tempo_curve(smooth_tempogram, std::get<1>(cyclic_tempgram));
    tempo_curve = curve_utils::correct_curve_by_length(tempo_curve, 40);

    auto tempo_segments = curve_utils::split_curve(tempo_curve);
    auto tempo_sections_tmp = curve_utils::tempo_segments_to_sections(tempo_segments, tempo_curve, std::get<2>(tempogram_tpl),DEFAULT_REF_TEMPO );
    std::vector<curve_utils::Section> tempo_sections;
    for(const auto &section : tempo_sections_tmp) curve_utils::split_section(section, tempo_sections, 60);

    for(auto &section : tempo_sections) {
        curve_utils::extract_offset(std::get<0>(nov_cv), section, {1, 2, 4}, std::get<1>(nov_cv));
    }

    for(auto &section : tempo_sections) {
        std::cout << section << std::endl;
    }

    return 0;
}