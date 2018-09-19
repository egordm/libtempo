//
// Created by egordm on 29-8-2018.
//

#ifndef PROJECT_TEMPOGRAM_UTILS_H
#define PROJECT_TEMPOGRAM_UTILS_H

#include <armadillo>

using namespace arma;

namespace tempogram { namespace tempogram_utils {
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
     * Applies a window to the tempogram sums up the result of each window and uses these to generate a more
     * smoothed tempogram.
     *
     * @param tempogram
     * @param window
     * @return
     */
    mat tempogram_apply_window(const mat &tempogram, const vec &window);

    /**
     * Sums the bins with their corresponding triplet bins. triplet = 3/2 tempo
     *
     * @param tempogram
     * @param axis_lut
     * @param weight
     * @return
     */
    mat tempogram_include_triplets(const mat &tempogram, const vec &axis_lut, float weight = 2.f);

    /**
     * Smoothens tempogram to prepare it for peak extraction.
     * It adds triplets to the intensities, does temporal accumulation, denoising and normalization.
     *
     * @param tempogram
     * @param axis_lut
     * @param temporal_unit_size: length over which the tempogram will be stabilized to extract a steady tempo.
     * @param triplet_weight: weight of the triplet intensity which will be adeed to its base intensity
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

    /**
     * Convert bpm to cyclic bpm
     * @param bpm
     * @param ref_tempo
     * @return
     */
    double bpm_to_cyclic(double bpm, double ref_tempo = 60);

}}


#endif //PROJECT_TEMPOGRAM_UTILS_H
