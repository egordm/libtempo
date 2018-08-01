//
// Created by egordm on 30-7-2018.
//

#include "resample.h"
#include "upfirdn.h"
#include "defines.h"
#include "math_utils.hpp"
#include <sigpack.h>
#include <cmath>


using namespace sp;
using namespace tempogram::utils;

int calc_gcd(int n1, int n2) {
    int tmp = 0;
    while (n1 > 0) {
        tmp = n1;
        n1 = n2 % n1;
        n2 = tmp;
    }
    return n2;
}

int quotient_ceil(int n1, int n2) {
    if (n1 % n2 != 0) return n1 / n2 + 1;
    return n1 / n2;
}

vec firls(int length, vec freq, const vec &amplitude) {
    int filter_length = length + 1;
    length = length / 2;

    vec weight(freq.size() / 2, fill::ones);
    freq /= 2.;
    vec freqd = diff(freq);

    int Nodd = filter_length % 2;
    auto k_size = static_cast<const uword>(length + 1);
    vec k = regspace(0, length);
    if (Nodd == 0) k += 0.5;

    double b0 = 0.0;
    vec b(k_size, fill::zeros);
    for (uword i = 0; i < freq.size(); i += 2) {
        uword ii = i + 1;
        double w_half_q = weight.at((ii) / 2) * weight.at((ii) / 2);

        double slope = (amplitude.at(ii) - amplitude.at(i)) / freqd.at(i); // TODO: freq diff
        double b1 = amplitude.at(i) - slope * freq.at(i);

        if (Nodd == 1) {
            b0 += (b1 * freqd.at(i)) + slope / 2.0 * (freq.at(ii) * freq.at(ii) - freq.at(i) * freq.at(i)) * w_half_q;
        }

        b += slope / (M_2PI * M_2PI) * (cos(M_2PI * k * freq.at(i + 1)) - cos(M_2PI * k * freq.at(i))) / (k % k);
        b += (freq.at(ii) * (slope * freq.at(ii) + b1) * math::sinc_fac(2 * k * freq.at(ii)))
             - (freq.at(i) * (slope * freq.at(i) + b1) * math::sinc_fac(2 * k * freq.at(i)));
        b *= w_half_q;
    }

    if (Nodd == 1) b.at(0) = b0;
    double w0 = weight.at(0);
    vec a = w0 * w0 * 4 * b / 2;

    if (Nodd == 1) return join_cols(flipud(a), a(span(1, a.size() - 1)));
    else return join_cols(flipud(a), a);
}

vec tempogram::resample(const vec &signal, int upfactor, int downfactor) {
    const int n = 10;
    if (upfactor <= 0 || downfactor <= 0) throw std::runtime_error("factors must be positive integer");

    int gcd = calc_gcd(upfactor, downfactor);
    upfactor /= gcd;
    downfactor /= gcd;

    if (upfactor == downfactor) return signal;
    int max_factor = max(upfactor, downfactor);

    double firls_freq = 0.5 / max_factor;
    vec firls_freqs = { 0.0, 2.0 * firls_freq, 2.0 * firls_freq, 1.0 };
    vec firls_amplitude = { 1.0, 1.0, 0.0, 0.0 };
    int length = 2 * n * max_factor + 1;
    vec coefficients = firls(length - 1, firls_freqs, firls_amplitude);
    vec window = sp::kaiser((const uword)length, 5.0);
    coefficients %= upfactor * window;

    int length_half = (length - 1) / 2;
    int output_size = quotient_ceil((int)signal.size() * upfactor, downfactor);
    int nz = downfactor - length_half % downfactor;
    vec h = join_cols(vec((const uword)nz, fill::zeros), coefficients);
    length_half += nz;
    int delay = length_half / downfactor;
    nz = (output_size + delay) * downfactor - (((int)signal.size() - 1) * upfactor + (int)h.size()); // solve out size
    if(nz > 0) h = join_cols(h, vec((const uword)nz, fill::zeros));

    vec resr = tempogram::upfirdn(signal, upfactor, downfactor, h);
    return resr(span((uword)delay, (uword) delay + output_size - 1));
}