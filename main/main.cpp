#include <iostream>
#include <audio_processing.h>
#include <tempogram.h>


int main() {

    arma::vec A(10);
    A.ones();
    arma::vec B = linspace<vec>(0, 9, 10);

    //auto test = tempogram::novelty_curve_to_tempogram_dft(A, B, 10, 10, 10);

    std::cout << B(span(0, 4)) << std::endl;

    auto audio = tempogram::audio::open_audio("../../valley_of.wav");
    mat reduced_sig = mean(audio.data, 0);
    vec signal = reduced_sig.row(0).t();



    tempogram::audio_to_novelty_curve(signal, audio.sr);
    std::cout << "Test" << std::endl;

    return 0;
}