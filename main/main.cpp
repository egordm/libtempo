#include <iostream>
#include <audio_processing.h>
#include <tempogram_processing.h>
#include <tempogram_utils.h>
#include <curve_utils.h>
#include <mat_utils.h>
#include <signal_utils.h>
#include <args.hxx>
#include "cli_utils.h"
#include "settings.h"
#include "flag_logic.h"
#include "present_utils.h"


using namespace std::chrono;
using namespace tempogram;
using namespace args;
using namespace present_utils;


int main(int argc, char **argv) {
    Settings settings;

    bool error = false, do_exit = false;
    parse_arguments(settings, argc, argv, do_exit, error);

    if (do_exit) exit(error ? 1 : 0);

    // Do program logic
    std::cout << "Processing " << settings.audio_file << std::endl;
    auto audio = audio::AudioFile::open(settings.audio_file.c_str());
    fmat reduced_sig = mean(audio.data, 0);
    fvec signal = reduced_sig.row(0).t();

    std::cout << " - Calculating novelty curve" << std::endl;
    int feature_rate;
    auto novelty_curve = tempogram_processing::audio_to_novelty_curve(feature_rate, signal, audio.sr);

    std::cout << " - Calculating tempogram" << std::endl;
    vec bpm = regspace(std::get<0>(settings.bpm_scan_window), std::get<1>(settings.bpm_scan_window));
    vec t;
    auto tempogram = tempogram_processing::novelty_curve_to_tempogram_dft
            (t, novelty_curve, bpm, feature_rate, settings.tempo_window);

    std::cout << " - Calculating cyclic tempogram" << std::endl;
    auto normalized_tempogram = tempogram::normalize_feature(tempogram, 2, 0.0001);
    int ref_tempo = settings.ref_tempo;
    vec ct_y_axis;
    auto cyclic_tempogram = tempogram_processing::tempogram_to_cyclic_tempogram
            (ct_y_axis, normalized_tempogram, bpm, settings.octave_divider, ref_tempo);

    std::cout << " - Preprocessing and cleaning tempogram" << std::endl;
    int smooth_length_samples = (int) (settings.smooth_length / (t[1] - t[0]));
    auto smooth_tempogram = tempogram_utils::smoothen_tempogram
            (cyclic_tempogram, ct_y_axis, smooth_length_samples, settings.triplet_weight);

    std::cout << " - Tempo peaks extraction" << std::endl;
    auto tempo_curve = tempogram_utils::extract_tempo_curve(smooth_tempogram, ct_y_axis);
    int min_section_length_samples = (int) (settings.min_section_length / (t[1] - t[0]));
    tempo_curve = curve_utils::correct_curve_by_length(tempo_curve, min_section_length_samples);

    auto tempo_segments = curve_utils::split_curve(tempo_curve);
    auto tempo_sections_tmp = curve_utils::tempo_segments_to_sections
            (tempo_segments, tempo_curve, t, settings.ref_tempo);

    // Merge sections for consistency
    tempo_sections_tmp = curve_utils::merge_sections(tempo_sections_tmp, settings.bpm_merge_threshold);

    // Split section for precision
    std::vector<curve_utils::Section> tempo_sections;
    for (const auto &section : tempo_sections_tmp)
        curve_utils::split_section(section, tempo_sections, settings.max_section_length);

    std::cout << " - Tempo offset estimation" << std::endl;
    for (auto &section : tempo_sections) {
        // BPM correction to preferred bpm
        int best_multiple = *std::min_element(settings.tempo_multiples.begin(), settings.tempo_multiples.end(),
                                              [&settings, &section](const int &a, const int &b) {
                                                  return fabs(settings.preferred_bpm - a * section.bpm) <
                                                         fabs(settings.preferred_bpm - b * section.bpm);
                                              });
        section.bpm = section.bpm * best_multiple;

        // Do bpm rounding
        int precision_multiple = (int) round(section.bpm / settings.bpm_rounding_precision);
        section.bpm = settings.bpm_rounding_precision * precision_multiple;

        curve_utils::extract_offset(novelty_curve, section, settings.tempo_multiples, feature_rate,
                                    settings.bpm_doubt_window, settings.bpm_doubt_step);
        curve_utils::correct_offset(section, 4);
    }
    std::cout << "Done!" << std::endl;

    std::cout << std::endl << "Found following tempo sections: " << std::endl;
    for (auto &section : tempo_sections) {
        std::cout << section << std::endl;
    }

    // Save audio click track
    if (settings.generate_click_track) {
        try {
            save_click_track(audio, tempo_sections, settings.click_track_subdivision);
        } catch (const std::runtime_error &error) {
            std::cerr << error.what() << std::endl;
            exit(1);
        }
    }

    std::string base_file = audio.path;
    audio::split_ext(base_file);

    if (settings.visualize) {
        std::cout << std::endl << "Writing visualization data" << std::endl;
        visualize(base_file, settings, novelty_curve, abs(normalized_tempogram), t, bpm, cyclic_tempogram, tempo_curve,
                  ct_y_axis, smooth_tempogram, ref_tempo, feature_rate, tempo_sections);
    }

    if (settings.dump_data) {
        std::cout << std::endl << "Dumping useful data" << std::endl;
        dump(base_file, settings, novelty_curve, abs(normalized_tempogram), t, bpm, cyclic_tempogram, tempo_curve,
             ct_y_axis, smooth_tempogram, ref_tempo, feature_rate, tempo_sections);
    }

    if (settings.format_for_osu) {
        std::cout << std::endl << "Osu timing points" << std::endl;
        for (const auto &section : tempo_sections) std::cout << section_to_osu(section) << std::endl;
    }
}