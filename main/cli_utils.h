//
// Created by egordm on 5-9-2018.
//

#ifndef PROJECT_CLI_UTILS_H
#define PROJECT_CLI_UTILS_H

#include <tuple>
#include <vector>
#include <args.hxx>
#include "settings.h"

using namespace args;

struct IntsReader {
    void operator()(const std::string &name, const std::string &value, std::tuple<int, int> &destination) {
        size_t commapos = value.find(',');
        if (commapos == std::string::npos) return;

        std::string v1 = value.substr(0, commapos);
        std::string v2 = value.substr(commapos + 1, value.size() - commapos - 1);

        std::get<0>(destination) = std::stoi(v1);
        std::get<1>(destination) = std::stoi(v2);
    }
};

#define TEMPO_WINDOW_DESC "Analysis window length in seconds for calculating tempogram"
#define BPM_WINDOW_DESC "BPM window which to check for tempo peaks"
#define REF_TEMPO_DESC "Reference tempo defining the partition of BPM into tempo octaves for calculating cyclic tempogram"
#define OCTAVE_DIVIDER_DESC "Number of tempo classes used for representing a tempo octave. This parameter controls the dimensionality of cyclic tempogram"
#define SMOOTH_LENGTH_DESC "Length over which the tempogram will be stabilized to extract a steady tempo"
#define TRIPLET_WEIGH_DESC "Weight of the triplet intensity which will be adeed to its base intensity"
#define MIN_SECTION_LENGTH_DESC "Minimum length for a tempo section in samples"
#define MAX_SECTION_LENGTH_DESC "Maximum section length in seconds after which section is split in half"
#define BPM_DOUBT_WINDOW_DESC "Window around candidate bpm which to search for a more fine and correct bpm"
#define BPM_DOUBT_STEP_DESC "Steps which to take inside the doubt window to fine tune the bpm"
#define TEMPO_MULTIPLES_DESC "Tempo multiples to consider when searching for correct offset"
#define GENERATE_CLICK_TRACK_DESC "Wether or not a click track should be generated"
#define CLICK_TRACK_SUBDIVISION_DESC "Click subdivision for the click track"
#define OSU_DESC "Wether or not to generate tempo data in osu format."
#define VIZ_DESC "Saves all the generated structures to a binary fromat for manual visualization."
#define AUDIO_DESC "Audio file to extract tempo of."


void parse_arguments(Settings &settings, int argc, char **argv, bool &exit, bool &error);

struct Applyable { // IDK
    virtual ~Applyable() = default;

    virtual void apply() = 0;
};

template<typename T>
struct SettingArg : Applyable {
    T &setting;
    ValueFlag<T> arg;

    SettingArg(Group &group, const std::string &name, const std::string &help, Matcher &&matcher, T &setting)
            : setting(setting), arg(group, name, help, std::move(matcher), setting) {}

    inline void apply() override {
        if (arg) setting = get(arg);
    }
};


#endif //PROJECT_CLI_UTILS_H
