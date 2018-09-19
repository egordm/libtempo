//
// Created by egordm on 28-7-2018.
//

#include "audio_processing.h"

#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_IMPLEMENTATION

#include <minimp3.h>

using namespace tempogram;

audio::AudioFile audio::AudioFile::open(const char *path) {
    std::string base = path;
    std::string ext = split_ext(base);
    
    if(ext == ".mp3") {
        return audio::open_lossy(path);
    } else {
        return audio::open_lossless(path);
    }
}

void audio::AudioFile::save(const char *path) const {
    SF_INFO sfinfo;
    sfinfo.channels = (int) data.n_rows;
    sfinfo.samplerate = sr;
    sfinfo.format = format;
    sfinfo.frames = data.n_cols;
    sfinfo.sections = 1;
    sfinfo.seekable = 1;

    SNDFILE *sndfile = sf_open(path, SFM_WRITE, &sfinfo);

    if (sndfile == nullptr) {
        int err = sf_error(sndfile);
        const char *err_str = sf_error_number(err);
        std::stringstream ss;
        ss << "Cant open audio file : " << path << " Recieved error code " << err << " Error status: " << err_str
           << std::endl;
        std::cout << ss.str() << std::endl;
        throw std::runtime_error(ss.str());
    }

    sf_count_t count = sf_write_float(sndfile, data.memptr(), data.size());
    sf_write_sync(sndfile);
    sf_close(sndfile);
}

audio::AudioFile audio::open_lossless(const char *path) {
    SF_INFO sfinfo;
    SNDFILE *sndfile = sf_open(path, SFM_READ, &sfinfo);

    if (sndfile == nullptr) {
        int err = sf_error(sndfile);
        const char *err_str = sf_error_number(err);
        std::stringstream ss;
        ss << "Cant open audio file : " << path << " Recieved error code " << err << " Error status: " << err_str
           << std::endl;
        std::cout << ss.str() << std::endl;
        throw std::runtime_error(ss.str());
    }

    fmat data((const uword) sfinfo.channels, (const uword) sfinfo.frames);
    sf_read_float(sndfile, data.memptr(), sfinfo.channels * sfinfo.frames);

    audio::AudioFile ret(path, data, sfinfo.samplerate, sfinfo.format);

    sf_close(sndfile);
    return ret;
}

audio::AudioFile audio::open_lossy(const char *path) {
    // Read the file
    std::ifstream ifs(path, ios::binary | ios::ate);
    if (!ifs) {
        std::cerr << "Failed to read file: " << path << std::endl;
        throw std::runtime_error("FILE READ ERROR");
    }

    std::ifstream::pos_type pos = ifs.tellg();

    std::vector<char> result(pos);
    ifs.seekg(0, ios::beg);
    ifs.read(result.data(), pos);
    ifs.close();

    // Init minimp3 vars
    unsigned char *input_buf = reinterpret_cast<unsigned char *>(result.data());
    int input_buf_size = static_cast<int>(result.size());

    mp3dec_t mp3d;
    mp3dec_init(&mp3d);

    mp3dec_frame_info_t frame_info;
    std::vector<float> frame_data(MINIMP3_MAX_SAMPLES_PER_FRAME);

    std::vector<float> pcm;
    int sample_count = 0;

    // Read per frame and uncompress to pcm
    do {
        int samples = mp3dec_decode_frame(&mp3d, input_buf, input_buf_size, frame_data.data(), &frame_info);
        if (samples) {
            std::copy(frame_data.data(), frame_data.data() + (samples * frame_info.channels), std::back_inserter(pcm));
        }

        input_buf += frame_info.frame_bytes;
        input_buf_size -= frame_info.frame_bytes;
        sample_count += samples;
    } while (frame_info.frame_bytes);

    // Convert to matrix
    fmat audio_data(pcm.data(), frame_info.channels, sample_count);
    return audio::AudioFile(path, audio_data, frame_info.hz);
}

std::string audio::split_ext(std::string &path) {
    std::string ext;
    size_t ext_pos = path.find_last_of('.');

    if (ext_pos != std::string::npos) {
        ext = path.substr(ext_pos, path.size() - ext_pos);
        path = path.substr(0, ext_pos);
    }

    return ext;
}
