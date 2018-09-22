//
// Created by egordm on 30-7-2018.
//

#include "resample.h"
#include "defines.h"
#include "math_utils.hpp"


using namespace sp;
using namespace libtempo;

vec libtempo::resample(const vec &signal, int upfactor, int downfactor) {
    const int n = 10;
    if (upfactor <= 0 || downfactor <= 0) throw std::runtime_error("factors must be positive integer");

    int gcd = utils::math::calc_gcd(upfactor, downfactor);
    upfactor /= gcd;
    downfactor /= gcd;

    if (upfactor == downfactor) return signal;
    int max_factor = std::max(upfactor, downfactor);

    double firls_freq = 0.5 / max_factor;
    vec firls_freqs = { 0.0, 2.0 * firls_freq, 2.0 * firls_freq, 1.0 };
    vec firls_amplitude = { 1.0, 1.0, 0.0, 0.0 };
    int length = 2 * n * max_factor + 1;
    vec coefficients = firls(length - 1, firls_freqs, firls_amplitude);
    vec window = sp::kaiser((const uword)length, 5.0);
    coefficients %= upfactor * window;

    int length_half = (length - 1) / 2;
    int output_size = utils::math::quotient_ceil((int)signal.size() * upfactor, downfactor);
    int nz = downfactor - length_half % downfactor;
    vec h = join_cols(vec((const uword)nz, fill::zeros), coefficients);
    length_half += nz;
    int delay = length_half / downfactor;
    nz = (output_size + delay) * downfactor - (((int)signal.size() - 1) * upfactor + (int)h.size()); // solve out size
    if(nz > 0) h = join_cols(h, vec((const uword)nz, fill::zeros));

    vec resr = libtempo::upfirdn(signal, upfactor, downfactor, h);
    return resr(span((uword)delay, (uword) delay + output_size - 1));
}

vec libtempo::firls(int length, vec freq, const vec &amplitude) {
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
        b += (freq.at(ii) * (slope * freq.at(ii) + b1) * utils::math::sinc_fac(2 * k * freq.at(ii)))
             - (freq.at(i) * (slope * freq.at(i) + b1) * utils::math::sinc_fac(2 * k * freq.at(i)));
        b *= w_half_q;
    }

    if (Nodd == 1) b.at(0) = b0;
    double w0 = weight.at(0);
    vec a = w0 * w0 * 4 * b / 2;

    if (Nodd == 1) return join_cols(flipud(a), a(span(1, a.size() - 1)));
    else return join_cols(flipud(a), a);
}

libtempo::Resampler::Resampler(int up_rate, int down_rate, const vec &coefs)
        :  _up_rate(up_rate), _down_rate(down_rate), _t(0), _xoffset(0) {
    /*
    The coefficients are copied into local storage in a transposed, flipped
    arrangement.  For example, suppose upRate is 3, and the input number
    of coefficients coefCount = 10, represented as h[0], ..., h[9].
    Then the internal buffer will look like this:
        h[9], h[6], h[3], h[0],   // flipped phase 0 coefs
           0, h[7], h[4], h[1],   // flipped phase 1 coefs (zero-padded)
           0, h[8], h[5], h[2],   // flipped phase 2 coefs (zero-padded)
    */
    _coefs_per_phase = (int) ceil((float) coefs.size() / up_rate);
    _padded_coef_count = _coefs_per_phase * up_rate;

    _state = new double[_coefs_per_phase - 1];
    _state_end = _state + _coefs_per_phase - 1;
    std::fill(_state, _state_end, 0.);

    _transposed_coefs = new double[_padded_coef_count];
    std::fill(_transposed_coefs, _transposed_coefs + _padded_coef_count, 0.);

    /* This both transposes, and "flips" each phase, while
    * copying the defined coefficients into local storage.
    * There is probably a faster way to do this
    */
    for (int i = 0; i < _up_rate; ++i)
        for (int j = 0; j < _coefs_per_phase; ++j) {
            if (j * _up_rate + i < coefs.size())
                _transposed_coefs[(_coefs_per_phase - 1 - j) + i * _coefs_per_phase] = coefs.at(j * _up_rate + i);
        }
}

libtempo::Resampler::~Resampler() {
    delete [] _transposed_coefs;
    delete [] _state;
}

int libtempo::Resampler::out_count(int in_count) {
    int np = in_count * _up_rate;
    int need = np / _down_rate;
    if ((_t + _up_rate * _xoffset) < (np % _down_rate))  need++;
    return need;
}

vec libtempo::Resampler::apply(vec &in) {
    vec ret((const uword)(out_count((int)in.size())));
    // x points to the latest processed input sample
    double *start = in.memptr();
    double *x = start + _xoffset;
    double *end = start + in.size();
    double *y = ret.memptr();

    while (x < end) {// TODO: with some smarter coding this can be parallellized?
        double acc = 0.;
        double *h = _transposed_coefs + _t * _coefs_per_phase;
        double *x_ptr = x - _coefs_per_phase + 1;
        long offset = (long)(start - x_ptr);

        if (offset > 0) {
            // need to draw from the _state buffer
            double *state_ptr = _state_end - offset;
            while (state_ptr < _state_end)  acc += *state_ptr++ * *h++;
            x_ptr += offset;
        }
        while (x_ptr <= x) acc += *x_ptr++ * *h++;

        *y++ = acc;
        _t += _down_rate;

        int advance_amount = _t / _up_rate;
        x += advance_amount;
        // which phase of the filter to use
        _t %= _up_rate;
    }
    _xoffset = static_cast<int>(x - end);

    // manage _state buffer
    // find number of samples retained in buffer:
    int retain = (_coefs_per_phase - 1) - (int)in.size();
    if (retain > 0) {
        // for inCount smaller than state buffer, copy end of buffer to beginning:
        std::copy(_state_end - retain, _state_end, _state);
        // Then, copy the entire (short) input to end of buffer
        std::copy(start, end, _state_end - (int)in.size());
    } else {
        // just copy last input samples into state buffer
        std::copy(end - (_coefs_per_phase - 1), end, _state);
    }

    return ret;
}


vec libtempo::upfirdn(const vec &signal, int up_rate, int down_rate, const vec &filter) {
    Resampler resampler(up_rate, down_rate, filter);

    // pad input by length of one polyphase of filter to flush all values out
    int padding = resampler.coefs_per_phase() - 1;
    vec input = join_cols(signal, vec((const uword)padding, fill::zeros));

    return resampler.apply(input);
}