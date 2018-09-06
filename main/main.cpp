#include <iostream>
#include <audio_processing.h>
#include <tempogram_processing.h>
#include <tempogram_utils.h>
#include <curve_utils.h>
#include <generic_algothms.h>
#include <signal_utils.h>
#include <args.hxx>
#include "cli_utils.h"
#include "present_utils.h"
#include <filesystem>
#include "settings.h"

namespace fs = std::filesystem;

using namespace std::chrono;
using namespace tempogram;
using namespace args;
using namespace present_utils;

int main(int argc, char **argv) {
    Settings settings;

    bool error, do_exit;
    parse_arguments(settings, argc, argv, do_exit, error);

    if(do_exit) exit(error ? 1 : 0);

    // Do program logic
    std::cout << "Processing " << settings.audio_file << std::endl;
    auto audio = tempogram::audio::open_audio(settings.audio_file.c_str());
    mat reduced_sig = mean(audio.data, 0);
    vec signal = reduced_sig.row(0).t();

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
    auto cyclic_tempgram = tempogram_processing::tempogram_to_cyclic_tempogram
            (ct_y_axis, normalized_tempogram, bpm, settings.octave_divider, ref_tempo);

    std::cout << " - Preprocessing and cleaning tempogram" << std::endl;
    int smooth_length_samples =(int)(settings.smooth_length / (t[1] - t[0]));
    auto smooth_tempogram = tempogram_utils::smoothen_tempogram
            (cyclic_tempgram, ct_y_axis, smooth_length_samples, settings.triplet_weight);

    std::cout << " - Tempo peaks extraction" << std::endl;
    auto tempo_curve = tempogram_utils::extract_tempo_curve(smooth_tempogram, ct_y_axis);
    int min_section_length_samples = (int)(settings.min_section_length / (t[1] - t[0]));
    tempo_curve = curve_utils::correct_curve_by_length(tempo_curve, min_section_length_samples);

    auto tempo_segments = curve_utils::split_curve(tempo_curve);
    auto tempo_sections_tmp = curve_utils::tempo_segments_to_sections
            (tempo_segments, tempo_curve, t, settings.ref_tempo);

    std::vector<curve_utils::Section> tempo_sections;
    for (const auto &section : tempo_sections_tmp)
        curve_utils::split_section(section, tempo_sections, settings.max_section_length);

    std::cout << " - Tempo offset estimation" << std::endl;
    auto tempo_multiples = settings.tempo_multiples;
    if (tempo_multiples.empty()) tempo_multiples = {1, 2, 4};
    for (auto &section : tempo_sections) {
        section.bpm *= 2;

        // Do bpm rounding
        int precision_multiple = (int)round(section.bpm / settings.bpm_rounding_precision);
        section.bpm = settings.bpm_rounding_precision * precision_multiple;

        curve_utils::extract_offset(novelty_curve, section, tempo_multiples, feature_rate, settings.bpm_doubt_window,
                                    settings.bpm_doubt_step);
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
        } catch (const std::runtime_error& error) {
            std::cerr << error.what() << std::endl;
            exit(1);
        }
    }

    if (settings.format_for_visualization) {
        std::cout << std::endl << "Writing results to a file" << std::endl;
        std::string base_file = audio.path;
        split_ext(base_file);
        base_file += "/";
        fs::create_directories(base_file.c_str());

        write_matrix_data(base_file + "novelty_curve.npd", novelty_curve, (char) (TYPE_DOUBLE),
                          (char *) &feature_rate, sizeof(feature_rate));
        write_matrix_data(base_file + "tempogram.npd", tempogram, (char) (TYPE_DOUBLE | TYPE_COMPLEX));
        write_matrix_data(base_file + "t.npd", t, (char) (TYPE_DOUBLE));
        write_matrix_data(base_file + "bpm.npd", bpm, (char) (TYPE_DOUBLE));
        write_matrix_data(base_file + "tempogram_cyclic.npd", cyclic_tempgram, (char) (TYPE_DOUBLE),
                          (char *) &ref_tempo, sizeof(ref_tempo));
        write_matrix_data(base_file + "ct_y_axis.npd", ct_y_axis, (char) (TYPE_DOUBLE));
        write_matrix_data(base_file + "smooth_tempogram.npd", smooth_tempogram, (char) (TYPE_DOUBLE),
                          (char *) &smooth_length_samples, sizeof(smooth_length_samples));
        write_matrix_data(base_file + "t_smooth.npd", t, (char) (TYPE_DOUBLE));
        write_matrix_data(base_file + "tempo_curve.npd", tempo_curve, (char) (TYPE_DOUBLE),
                          (char *) &settings.min_section_length, sizeof(settings.min_section_length));

        write_sections(base_file + "sections.txt", tempo_sections);
    }

    if (settings.format_for_osu) {
        std::cout << std::endl << "Osu timing points" << std::endl;
        for (const auto &section : tempo_sections) std::cout << section_to_osu(section) << std::endl;
    }
}