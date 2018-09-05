//
// Created by egordm on 5-9-2018.
//

#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include <tuple>
#include <vector>

struct Settings {
    int tempo_window = 8;
    std::tuple<int, int> bpm_window = {30, 600};
    int ref_tempo = 60;
    int octave_divider = 120;
    int smooth_length = 100;
    float triplet_weight = 3;
    int min_section_length = 40;
    int max_section_length = 40;
    float bpm_doubt_window = 2.f;
    float bpm_doubt_step = 0.1f;
    std::vector<int> tempo_multiples = {1, 2, 4};
    bool generate_click_track = true;
    int click_track_subdivision = 4;
    bool format_for_osu = false;
    bool format_for_visualization = false;

    std::string audio_file;
};

#endif //PROJECT_SETTINGS_H
