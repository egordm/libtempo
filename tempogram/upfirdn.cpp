//
// Created by egordm on 1-8-2018.
//

#include "upfirdn.h"

tempogram::Resampler::Resampler(int up_rate, int down_rate, const vec &coefs)
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

tempogram::Resampler::~Resampler() {
    delete [] _transposed_coefs;
    delete [] _state;
}

int tempogram::Resampler::out_count(int in_count) {
    int np = in_count * _up_rate;
    int need = np / _down_rate;
    if ((_t + _up_rate * _xoffset) < (np % _down_rate))  need++;
    return need;
}

vec tempogram::Resampler::apply(vec &in) {
    vec ret((const uword)(out_count((int)in.size())));
    // x points to the latest processed input sample
    double *start = in.memptr();
    double *x = start + _xoffset;
    double *end = start + in.size();
    double *y = ret.memptr();

    while (x < end) {
        double acc = 0.;
        double *h = _transposed_coefs + _t * _coefs_per_phase;
        double *x_ptr = x - _coefs_per_phase + 1;
        long offset = start - x_ptr;

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


vec tempogram::upfirdn(const vec &signal, int up_rate, int down_rate, const vec &filter) {
    Resampler resampler(up_rate, down_rate, filter);

    // pad input by length of one polyphase of filter to flush all values out
    int padding = resampler.coefs_per_phase() - 1;
    vec input = join_cols(signal, vec((const uword)padding, fill::zeros));

    return resampler.apply(input);
}