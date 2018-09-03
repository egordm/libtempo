//
// Created by egordm on 28-7-2018.
//

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

    tempogram::audio::AudioFile ret(path, data, sfinfo.samplerate, sfinfo.format);

    sf_close(sndfile);
    return ret;
}

void tempogram::audio::AudioFile::save(const char *path) const {
    SF_INFO sfinfo;
    sfinfo.channels = (int) data.n_rows;
    sfinfo.samplerate = sr;
    sfinfo.format = format;

    SNDFILE *sndfile = sf_open(path, SFM_WRITE, &sfinfo);

    if (sndfile == nullptr) {
        std::stringstream ss;
        ss << "Cant open audio file: " << path << endl;
        throw std::runtime_error(ss.str());
    }

    sf_count_t count = sf_write_double(sndfile, data.memptr(), data.size());
    sf_write_sync(sndfile);
    sf_close(sndfile);
}
