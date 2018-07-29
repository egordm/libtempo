#include <iostream>
#include <audio_processing.h>
#include <tempogram.h>


int main() {
    auto audio = tempogram::audio::open_audio("../../valley_of.wav");
    mat reduced_sig = mean(audio.data, 0);
    vec signal = reduced_sig.row(0).t();

    /*mat A(3,4, fill::randu);
    std::cout << A << std::endl << std::endl;
    std::cout << flipud(fliplr(A)) << std::endl;*/


    tempogram::audio_to_novelty_curve(signal, audio.sr);
    std::cout << "Test" << std::endl;

    return 0;
}