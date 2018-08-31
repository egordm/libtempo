//
// Created by egordm on 28-7-2018.
//

#ifndef PROJECT_AUDIO_PROCESSING_H
#define PROJECT_AUDIO_PROCESSING_H

#include <sndfile.h>
#include <armadillo>

using namespace arma;

namespace tempogram { namespace audio {
    struct AudioFile {
        mat data;
        int sr;

        AudioFile(const mat &data, int sr) : data(data), sr(sr) {}

        void save(const char *path, int format = SF_FORMAT_WAV | SF_FORMAT_PCM_24);
    };

    AudioFile open_audio(const char *path);
}};


#endif //PROJECT_AUDIO_PROCESSING_H
