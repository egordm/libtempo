//
// Created by egordm on 29-8-2018.
//

#ifndef PROJECT_GENERIC_ALGOTHMS_H
#define PROJECT_GENERIC_ALGOTHMS_H

#include <armadillo>

using namespace arma;

namespace tempogram { namespace mat_utils {
    /**
     * Finds index of nearest element to given value
     * @param a
     * @param value
     * @return
     */
    uword find_nearest(const vec &a, double value);

    /**
    * Subtracts mean from the tempogram and takes all positivbe results to reduce the noise.
    * @param tempogram
    * @return
    */
    mat subtract_mean(const mat &tempogram);

    /**
     * Normalize tempogram. Sum of bins at every time position will be 1
     * @param tempogram
     * @return
     */
    mat rowwise_normalize(const mat &tempogram);

    /**
     * Puts bin with max intensity at every time position in a vector.
     * @param tempogram
     * @return
     */
    uvec argmax(const mat &tempogram);

    /**
     * Puts value with max intensity at every time position in a vector.
     * @param tempogram
     * @param axis_lut
     * @return
     */
    vec max_bucket(const mat &tempogram, const vec &axis_lut);

    /**
     * Pads given vector with specified amount of zeros
     * @param x
     * @param n_before
     * @param n_after
     * @return
     */
    vec pad_vec(const vec &x, unsigned int n_before, unsigned int n_after);
}}


#endif //PROJECT_GENERIC_ALGOTHMS_H
