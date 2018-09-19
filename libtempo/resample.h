//
// Created by egordm on 30-7-2018.
//

#ifndef PROJECT_RESAMPLE_H
#define PROJECT_RESAMPLE_H

#include <armadillo>

using namespace arma;

namespace libtempo {
    class Resampler {
    public:
        Resampler(int up_rate, int down_rate, const vec &coefs);

        virtual ~Resampler();

        /**
         * compute how many outputs will be generated for n inputs
         * @param in_count
         * @return
         */
        int out_count(int in_count);

        vec apply(vec &in);

        int coefs_per_phase() { return _coefs_per_phase; }

    private:
        int _up_rate;
        int _down_rate;

        double *_transposed_coefs;
        double *_state;
        double *_state_end;

        int _padded_coef_count;  // ceil(len(coefs)/upRate)*upRate
        int _coefs_per_phase;    // _paddedCoefCount / upRate

        int _t;                // "time" (modulo upRate)
        int _xoffset;
    };

    /**
     * This template function provides a one-shot resampling. Extra samples are padded to the end of the input in
     * order to capture all of the non-zero output samples.
     * Thanks to Lewis Anderson (lkanders@ucsd.edu) at UCSD for the original version of this function.
     * @param signal
     * @param up_rate
     * @param down_rate
     * @param filter
     * @return
     */
    vec upfirdn(const vec &signal, int up_rate, int down_rate, const vec &filter);

    /**
     * firls designs a linear-phase FIR filter that minimizes the weighted, integrated squared error between an ideal
     * piecewise linear function and the magnitude response of the filter over a set of desired frequency bands.
     * @param length
     * @param freq
     * @param amplitude
     * @return
     */
    vec firls(int length, vec freq, const vec &amplitude);

    /**
     * resamples the input sequence, x, at p/q times the original sample rate.
     * @param signal
     * @param upfactor
     * @param downfactor
     * @return
     */
    vec resample(const vec &signal, int upfactor, int downfactor);
}


#endif //PROJECT_RESAMPLE_H
