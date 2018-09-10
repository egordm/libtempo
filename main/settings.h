//
// Created by egordm on 5-9-2018.
//

#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include <tuple>
#include <vector>

struct Settings {
    int preferred_bpm = 130;
    float bpm_rounding_precision = 0.5f;
    int tempo_window = 8;
    std::tuple<int, int> bpm_scan_window = {30, 600};
    int ref_tempo = 60;
    int octave_divider = 120;
    double smooth_length = 20;
    float triplet_weight = 3;
    double min_section_length = 10;
    double max_section_length = 40;
    float bpm_doubt_window = 2.f;
    float bpm_doubt_step = 0.1f;
    std::vector<int> tempo_multiples = {1, 2, 4};
    bool generate_click_track = true;
    int click_track_subdivision = 4;
    bool format_for_osu = false;
    bool visualize = false;
    bool dump_data = false;

    std::string audio_file;
};

#define PREFERRED_BPM_DESC "BPM around which the real bpm will be chosen."
#define BPM_ROUNDING_PRECISION_DESC "Precision of the BPM before correction.."
#define TEMPO_WINDOW_DESC "Analysis window length in seconds for calculating tempogram."
#define BPM_SCAN_WINDOW_DESC "BPM range which to check for tempo peaks."
#define REF_TEMPO_DESC "Reference tempo defining the partition of BPM into tempo octaves for calculating cyclic tempogram."
#define OCTAVE_DIVIDER_DESC "Number of tempo classes used for representing a tempo octave. This parameter controls the dimensionality of cyclic tempogram."
#define SMOOTH_LENGTH_DESC "Length in seconds over which the tempogram will be stabilized to extract a steady tempo."
#define TRIPLET_WEIGH_DESC "Weight of the triplet intensity which will be adeed to its base intensity."
#define MIN_SECTION_LENGTH_DESC "Minimum length for a tempo section in seconds."
#define MAX_SECTION_LENGTH_DESC "Maximum section length in seconds after which section is split in half."
#define BPM_DOUBT_WINDOW_DESC "Window around candidate bpm which to search for a more fine and correct bpm."
#define BPM_DOUBT_STEP_DESC "Steps which to take inside the doubt window to fine tune the bpm."
#define TEMPO_MULTIPLES_DESC "Tempo multiples to consider when searching for correct offset"
#define GENERATE_CLICK_TRACK_DESC "Wether or not a click track should be generated"
#define CLICK_TRACK_SUBDIVISION_DESC "Click subdivision for the click track."
#define OSU_DESC "Wether or not to generate tempo data in osu format."
#define VIZ_DESC "Saves a html file with useful graphs."
#define DUMP_DESC "Dump generated tempograms and novelty curves into a file."
#define AUDIO_DESC "Audio file to extract tempo of."


#endif //PROJECT_SETTINGS_H
