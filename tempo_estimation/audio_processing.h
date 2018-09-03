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
        std::string path;
        mat data;
        int sr;
        int format;

        AudioFile(const std::string &path, const mat &data, int sr, int format = SF_FORMAT_WAV | SF_FORMAT_PCM_24)
                : path(path), data(data), sr(sr), format(format) {}

        void save(const char *path) const;
    };

    AudioFile open_audio(const char *path);
}};


#endif //PROJECT_AUDIO_PROCESSING_H
