#include <iostream>
#include <audio_processing.h>
#include <tempo_estimation.h>

using namespace std::chrono;

int main() {
    auto audio = tempogram::audio::open_audio("../../valley_of.wav");
    mat reduced_sig = mean(audio.data, 0);
    vec signal = reduced_sig.row(0).t();

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    auto nov_cv = tempogram::audio_to_novelty_curve(signal, audio.sr);

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto d1 = duration_cast<microseconds>( t2 - t1 ).count();
    std::cout << "Step 1: novelty " << d1/1000. << "ms" << std::endl;

    vec bpm = regspace(30, 600);
    auto ret = tempogram::novelty_curve_to_tempogram_dft(std::get<0>(nov_cv), bpm, std::get<1>(nov_cv), 8);

    high_resolution_clock::time_point t3 = high_resolution_clock::now();
    auto d2 = duration_cast<microseconds>( t3 - t2 ).count();
    std::cout << "Step 2: tempogr " << d2/1000. << "ms" << std::endl;

    auto normalized_tempogram = tempogram::normalize_feature(std::get<0>(ret), 2, 0.0001);

    high_resolution_clock::time_point t4 = high_resolution_clock::now();
    auto d3 = duration_cast<microseconds>( t4 - t3 ).count();
    std::cout << "Step 3: normali " << d3/1000. << "ms" << std::endl;

    return 0;
}