//
// Created by egordm on 21-9-2018.
//

#ifndef PROJECT_AUDIO_IO_WRAPPER_HPP
#define PROJECT_AUDIO_IO_WRAPPER_HPP

#include "wrapper_libtempo.hpp"
#include <audio_processing.h>

using namespace libtempo;
using namespace wrapper_libtempo;

namespace wrapper_audio_io {
    class AudioWrapper {
    private:
        audio::AudioFile audio;

    public:
        explicit AudioWrapper(const char *path) : audio(audio::AudioFile::open(path)) {}

        std::string get_path() { return audio.path; }

        int get_sr() { return audio.sr; }

        int get_format() { return audio.format; }

        MatrixWrapper<float> get_data() { return MatrixWrapper<float>(audio.data);}

        void set_data(const MatrixWrapper<float> &data) { audio.data = data.data;}

        void save(const char *path) {
            try {
                audio.save(path);
            } catch (const std::runtime_error &error) {
                std::cerr << error.what() << std::endl;
            }
        }
    };

}

#endif //PROJECT_AUDIO_IO_WRAPPER_HPP
