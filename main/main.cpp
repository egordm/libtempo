#include <iostream>
#include <audio_processing.h>
#include <tempogram_processing.h>
#include <tempogram_utils.h>
#include <curve_utils.h>
#include <generic_algothms.h>
#include <signal_utils.h>
#include <args.hxx>
#include "constants.h"
#include "present_utils.h"

using namespace std::chrono;
using namespace tempogram;
using namespace args;

int main(int argc, char **argv) {
    ArgumentParser parser(
            "CLI for tempo estimation developed by Egor Dmitriev.\nVisit github for library version.");
    HelpFlag help(parser, "help", "Display the help menu", {'h', "help"});
    ValueFlag<int> tempo_window_arg(parser, "tempo_window", TEMPO_WINDOW_DESC, {"tempo_window"}, 8);
    ValueFlagList<int> bpm_window_arg(parser, "bpm_window", BPM_WINDOW_DESC, {"bpm_window"}, {30, 600});
    ValueFlag<int> ref_tempo_arg(parser, "ref_tempo", REF_TEMPO_DESC, {"ref_tempo"}, 60);
    ValueFlag<int> octave_divider_arg(parser, "octave_divider", OCTAVE_DIVIDER_DESC, {"octave_divider"}, 120);
    ValueFlag<int> smooth_length_arg(parser, "smooth_length", SMOOTH_LENGTH_DESC, {"smooth_length"}, 100);
    ValueFlag<float> triplet_weigh_arg(parser, "triplet_weight", TRIPLET_WEIGH_DESC, {"triplet_weight"}, 0.8f);
    ValueFlag<int> min_section_length_arg(parser, "min_section_length", MIN_SECTION_LENGTH_DESC, {"min_section_length"},
                                          40);
    ValueFlag<int> max_section_length_arg(parser, "max_section_length", MAX_SECTION_LENGTH_DESC, {"max_section_length"},
                                          40);
    ValueFlag<float> bpm_doubt_window_arg(parser, "bpm_doubt_window", BPM_DOUBT_WINDOW_DESC, {"bpm_doubt_window"}, 2.f);
    ValueFlag<float> bpm_doubt_step_arg(parser, "bpm_doubt_step", BPM_DOUBT_STEP_DESC, {"bpm_doubt_step"}, 0.1f);
    ValueFlagList<int> tempo_multiples_arg(parser, "tempo_multiples", TEMPO_MULTIPLES_DESC, {"tempo_multiples", 'm'},
                                           {1, 2, 4});
    ValueFlag<bool> generate_click_track_arg(parser, "generate_click_track", GENERATE_CLICK_TRACK_DESC,
                                             {"generate_click_track", 'c'}, true);
    ValueFlag<int> click_track_subdivision_arg(parser, "click_track_subdivision", CLICK_TRACK_SUBDIVISION_DESC,
                                               {"click_track_subdivision"}, 4);
    Flag osu_arg(parser, "osu", OSU_DESC, {"osu"});
    Positional<std::string> audio_arg(parser, "audio", AUDIO_DESC);

    try {
        parser.ParseCLI(argc, argv);
    } catch (Help &) {
        std::cout << parser;
        return 0;
    } catch (ParseError &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    } catch (ValidationError &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    std::cout << "Processing " << get(audio_arg) << std::endl;
    auto audio = tempogram::audio::open_audio(get(audio_arg).c_str());

    mat reduced_sig = mean(audio.data, 0);
    vec signal = reduced_sig.row(0).t();

    std::cout << " - Calculating novelty curve" << std::endl;
    auto nov_cv = tempogram_processing::audio_to_novelty_curve(signal, audio.sr);

    std::cout << " - Calculating tempogram" << std::endl;
    auto bpm_window = get(bpm_window_arg);
    if (bpm_window.size() < 2) bpm_window = {30, 600};
    vec bpm = regspace(bpm_window[0], bpm_window[1]);
    auto tempogram_tpl = tempogram_processing::novelty_curve_to_tempogram_dft
            (std::get<0>(nov_cv), bpm, std::get<1>(nov_cv), get(tempo_window_arg));

    std::cout << " - Calculating cyclic tempogram" << std::endl;
    auto normalized_tempogram = tempogram::normalize_feature(std::get<0>(tempogram_tpl), 2, 0.0001);
    auto cyclic_tempgram = tempogram_processing::tempogram_to_cyclic_tempogram
            (normalized_tempogram, std::get<1>(tempogram_tpl), get(octave_divider_arg), get(ref_tempo_arg));

    std::cout << " - Preprocessing and cleaning tempogram" << std::endl;
    auto t = std::get<2>(tempogram_tpl);
    t = t(span((uword) get(smooth_length_arg), t.n_rows - 1));
    auto smooth_tempogram = tempogram_utils::smoothen_tempogram
            (std::get<0>(cyclic_tempgram), std::get<1>(cyclic_tempgram), get(smooth_length_arg),
             get(triplet_weigh_arg));

    std::cout << " - Tempo peaks extraction" << std::endl;
    auto tempo_curve = tempogram_utils::extract_tempo_curve(smooth_tempogram, std::get<1>(cyclic_tempgram));
    tempo_curve = curve_utils::correct_curve_by_length(tempo_curve, get(min_section_length_arg));

    auto tempo_segments = curve_utils::split_curve(tempo_curve);
    auto tempo_sections_tmp = curve_utils::tempo_segments_to_sections
            (tempo_segments, tempo_curve, t, get(ref_tempo_arg));

    std::vector<curve_utils::Section> tempo_sections;
    for (const auto &section : tempo_sections_tmp)
        curve_utils::split_section(section, tempo_sections, get(max_section_length_arg));

    std::cout << " - Tempo offset estimation" << std::endl;
    auto tempo_multiples = get(tempo_multiples_arg);
    if (tempo_multiples.empty()) tempo_multiples = {1, 2, 4};
    for (auto &section : tempo_sections) {
        section.bpm *= 2;
        curve_utils::extract_offset(std::get<0>(nov_cv), section, tempo_multiples, std::get<1>(nov_cv),
                                    get(bpm_doubt_window_arg), get(bpm_doubt_step_arg));
        curve_utils::correct_offset(section, 4);
    }
    std::cout << "Done!" << std::endl;

    std::cout << std::endl << "Found following tempo sections: " << std::endl;
    for (auto &section : tempo_sections) {
        std::cout << section << std::endl;
    }

    // Save audio click track
    if (get(generate_click_track_arg)) {
        std::string output_path = get(audio_arg);
        std::string ext;
        size_t ext_pos = output_path.find_last_of('.');
        if (ext_pos != std::string::npos) {
            ext = output_path.substr(ext_pos, output_path.size() - ext_pos);
            output_path = output_path.substr(0, ext_pos);
        }

        output_path += "_processed" + ext;

        std::cout << "Writing click track: " << output_path << std::endl;

        vec click_track(audio.data.n_cols);
        for (auto &section : tempo_sections) {
            auto len = (unsigned long) ((section.end - section.start) * audio.sr);
            vec clicks = signal_utils::generate_click_track(section.bpm, section.offset - section.start,
                                                            get(click_track_subdivision_arg), len, audio.sr);

            auto start = (unsigned long) (section.start * audio.sr);
            click_track(span(start, start + clicks.n_rows - 1)) = clicks;
        }

        for (int c = 0; c < audio.data.n_rows; ++c) {
            audio.data(c, span::all) = clamp(audio.data(c, span::all) + click_track.t(), -1, 1);
        }

        audio.save(output_path.c_str());
    }

    if(osu_arg) {
        std::cout << std::endl << "Osu timing points" << std::endl;
        for(const auto &section : tempo_sections) std::cout << present_utils::section_to_osu(section) << std::endl;
    }
}