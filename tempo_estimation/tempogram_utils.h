//
// Created by egordm on 29-8-2018.
//

#ifndef PROJECT_TEMPOGRAM_UTILS_H
#define PROJECT_TEMPOGRAM_UTILS_H

#include <armadillo>

using namespace arma;

namespace tempogram { namespace tempogram_utils {
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
    mat normalize_tempogram(const mat &tempogram);

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
     * Extracts confidence of the max peak measurement at every time position.
     *
     * Confidence is determined by how evenly intenity is distributed among the bins.
     * If intensity is concentrated in one bin, then confidence is high.
     * In mathematical terms confidence is propertional to the distance of the modal bin to the peak bin.
     *
     * @param tempogram
     * @param rollover
     * @return
     */
    vec extract_confidence(const mat &tempogram, const bool &rollover = true);

    /**
     * Each temporal position becomes the sum of the temporal_size amount of temporal positions before it.
     * TODO: mayby add weight to closer temoporal positions.
     *
     * @param tempogram
     * @param temporal_size
     * @return
     */
    mat accumulate_temporal(const mat &tempogram, int temporal_size);

    /**
     * Sums the bins with their corresponding triplet bins. triplet = 3/2 tempo
     *
     * @param tempogram
     * @param axis_lut
     * @param weight
     * @return
     */
    mat tempogram_include_triplets(const mat &tempogram, const vec &axis_lut, float weight = 0.5f);

    /**
     * Smoothens tempogram to prepare it for peak extraction.
     * It adds triplets to the intensities, does temporal accumulation, denoising and normalization.
     *
     * @param tempogram
     * @param axis_lut
     * @param temporal_unit_size
     * @param triplet_weight
     * @return
     */
    mat smoothen_tempogram(const mat &tempogram, const vec &axis_lut, int temporal_unit_size = 100,
                           float triplet_weight = 0.8f);

    /**
     * Creates curve with values of the bins with max intensities from the tempogram.
     * @param tempogram
     * @param axis_lut
     * @return
     */
    vec extract_tempo_curve(const mat &tempogram, const vec &axis_lut);

}}


#endif //PROJECT_TEMPOGRAM_UTILS_H
