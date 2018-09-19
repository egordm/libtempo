//
// Created by egordm on 29-8-2018.
//

#ifndef PROJECT_GENERIC_ALGOTHMS_H
#define PROJECT_GENERIC_ALGOTHMS_H

#include <armadillo>

using namespace arma;

namespace libtempo { namespace mat_utils {
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

    template<typename T>
    void mat_interp1_nearest(const Col<T>& XG, const Mat<T>& YG, const Col<T>& XI, Mat<T>& YI) {
        const T XG_min = XG.min();
        const T XG_max = XG.max();

        YI.set_size(XI.size(), YG.n_cols);

        const T* XG_mem = XG.memptr();
        const T* XI_mem = XI.memptr();

        const uword NG = XG.n_elem;
        const uword NI = XI.n_elem;

        uword best_j = 0;

        for(uword i=0; i<NI; ++i) {
            T best_err = Datum<T>::inf;
            const T XI_val = XI_mem[i];

            if(XI_val < XG_min) {
                YI(i, span::all) = YG(0, span::all);
            } else if (XI_val > XG_max) {
                YI(i, span::all) = YG(NG - 1, span::all);
            } else {
                for(uword j=best_j; j<NG; ++j)  {
                    const T tmp = XG_mem[j] - XI_val;
                    const T err = (tmp >= T(0)) ? tmp : -tmp;

                    if(err >= best_err)  {
                        // error is going up, so we have found the optimum position
                        break;
                    }
                    else {
                        best_err = err;
                        best_j   = j;   // remember the optimum position
                    }
                }
                YI(i, span::all) = YG(best_j, span::all);
            }
        }
    }
}}


#endif //PROJECT_GENERIC_ALGOTHMS_H
