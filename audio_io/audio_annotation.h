//
// Created by egordm on 1-10-2018.
//

#ifndef PROJECT_AUDIO_ANNOTATION_H
#define PROJECT_AUDIO_ANNOTATION_H

#include <armadillo>

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace arma;

namespace libtempo { namespace audio { namespace annotation {
    /**
     * Generates a click sound
     * @param sr
     * @param duration: click duration in seconds
     * @param freq
     * @return
     */
    fvec generate_click(int sr = 22050, double duration = 0.1, double freq = 1000.0);

    /**
     * Generates click track of given length for given click positions in secions
     * @param positions: click positions in seconds
     * @param length: length of the track in seconds
     * @param sr
     * @return
     */
    fvec generate_click_track(const std::vector<float> &positions, unsigned long length, int sr = 22050);


    /**
     * Generates a clicktrack by placing click on given notes of given bpm and offset.
     * @param bpm
     * @param offset
     * @param length
     * @param note_fraction
     * @param sr
     * @return
     */
    fvec generate_click_track(double bpm, double offset, unsigned long length, int note_fraction = 4, int sr = 22050);
}}}


#endif //PROJECT_AUDIO_ANNOTATION_H
