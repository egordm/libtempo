//
// Created by egordm on 3-9-2018.
//

#include <signal_utils.h>
#include "present_utils.h"

std::string present_utils::split_ext(std::string &path) {
    std::string ext;
    size_t ext_pos = path.find_last_of('.');

    if (ext_pos != std::string::npos) {
        ext = path.substr(ext_pos, path.size() - ext_pos);
        path = path.substr(0, ext_pos);
    }

    return ext;
}

std::string present_utils::section_to_osu(const curve_utils::Section &section) {
    std::stringstream ss;
    ss << (uword) (section.offset * 1000) << ","
       << (uword) (60000 / section.bpm) << ","
       << "4,1,0,45,1,0";

    return ss.str();
}

void present_utils::save_click_track(audio::AudioFile &audio, const std::vector<curve_utils::Section> &sections,
                                     int fraction) {

    std::string output_path = audio.path;
    std::string ext = split_ext(output_path);

    output_path += "_click" + ext;

    std::cout << "Writing click track: " << output_path << std::endl;
    fvec click_track(audio.data.n_cols, fill::zeros);
    for (auto &section : sections) {
        auto len = (unsigned long) ((section.end - section.start) * audio.sr);
        fvec clicks = signal_utils::generate_click_track(section.bpm, section.offset - section.start, fraction, len,
                                                        audio.sr);

        auto start = (unsigned long) (section.start * audio.sr);
        click_track(span(start, start + clicks.n_rows - 1)) = clicks;
    }

     for (int c = 0; c < audio.data.n_rows; ++c) {
        audio.data(c, span::all) = clamp(audio.data(c, span::all) + click_track.t(), -1, 1);
    }

    audio.save(output_path.c_str());
}

std::string present_utils::serialize_section(const curve_utils::Section &section) {
    std::stringstream ss;
    ss << section.start << "," << section.end << "," << section.bpm << "," << section.offset;
    return ss.str();
}

void present_utils::write_sections(const std::string &filename, const std::vector<curve_utils::Section> &sections) {
    std::ofstream file;
    file.open(filename);
    if (!file.is_open()) {
        std::cerr << "Cant open: " << filename << std::endl;
        exit(1);
    }

    for(const auto &section : sections) {
        file << serialize_section(section) << std::endl;
    }

    file.close();
}
