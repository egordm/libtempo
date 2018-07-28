//
// Created by egordm on 28-7-2018.
//

#include <sndfile.h>
#include "audio_processing.h"

tempogram::audio::AudioFile tempogram::audio::open_audio(const char *path) {
    SF_INFO sfinfo;
    SNDFILE *sndfile = sf_open(path, SFM_READ, &sfinfo);

    if (sndfile == nullptr) {
        std::stringstream ss;
        ss << "Cant open audio file: " << path << endl;
        throw std::runtime_error(ss.str());
    }

    mat data((const uword)sfinfo.channels, (const uword)sfinfo.frames);
    sf_read_double(sndfile, data.memptr(), sfinfo.channels * sfinfo.frames);

    tempogram::audio::AudioFile ret(data, sfinfo.samplerate);

    sf_close(sndfile);
    return ret;
}
