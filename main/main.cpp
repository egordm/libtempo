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
#include "plotly.h"

namespace fs = std::filesystem;
namespace plt = plotly;
using namespace std::chrono;
using namespace tempogram;
using namespace args;
using namespace present_utils;

int main(int argc, char **argv) {
    Settings settings;

    bool error, do_exit;
    parse_arguments(settings, argc, argv, do_exit, error);

    if (do_exit) exit(error ? 1 : 0);

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
    int smooth_length_samples = (int) (settings.smooth_length / (t[1] - t[0]));
    auto smooth_tempogram = tempogram_utils::smoothen_tempogram
            (cyclic_tempgram, ct_y_axis, smooth_length_samples, settings.triplet_weight);

    std::cout << " - Tempo peaks extraction" << std::endl;
    auto tempo_curve = tempogram_utils::extract_tempo_curve(smooth_tempogram, ct_y_axis);
    int min_section_length_samples = (int) (settings.min_section_length / (t[1] - t[0]));
    tempo_curve = curve_utils::correct_curve_by_length(tempo_curve, min_section_length_samples);

    auto tempo_segments = curve_utils::split_curve(tempo_curve);
    auto tempo_sections_tmp = curve_utils::tempo_segments_to_sections
            (tempo_segments, tempo_curve, t, settings.ref_tempo);

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
    split_ext(base_file);

    if (settings.visualize) {
        std::cout << std::endl << "Writing results to a file" << std::endl;
        plt::File viz_file;

        plt::Plot tempogram_plot("Tempogram");
        mat cleaned_tempogram = abs(normalized_tempogram);
        tempogram_plot.charts.push_back(plt::Chart::create_heatmap("tempogram", plt::arma_to_json(t),
                                                                   plt::arma_to_json(bpm),
                                                                   plt::arma_to_json(cleaned_tempogram)));
        for (const auto &multiple : settings.tempo_multiples) {
            std::string label = multiple == 1 ? "Tempo ref" : "Tempo " + std::to_string(multiple) + "x";
            tempogram_plot.charts.emplace_back(label, plt::arma_to_json(t),
                                               plt::arma_to_json((vec) (tempo_curve * ref_tempo * multiple)));
        }
        viz_file.plots.push_back(tempogram_plot);

        plt::Plot cyclic_tempogram_plot("Cyclic Tempogram | Ref tempo = " + std::to_string(ref_tempo));
        cyclic_tempogram_plot.charts.push_back(plt::Chart::create_heatmap("tempogram", plt::arma_to_json(t),
                                                                   plt::arma_to_json(ct_y_axis),
                                                                   plt::arma_to_json(cyclic_tempgram)));
        cyclic_tempogram_plot.charts.emplace_back("peak tempo", plt::arma_to_json(t), plt::arma_to_json(tempo_curve));
        viz_file.plots.push_back(cyclic_tempogram_plot);

        plt::Plot smooth_tempogram_plot("Smoothed Tempogram | Smooth length = " + std::to_string(settings.smooth_length));
        smooth_tempogram_plot.charts.push_back(plt::Chart::create_heatmap("tempogram", plt::arma_to_json(t),
                                                                          plt::arma_to_json(ct_y_axis),
                                                                          plt::arma_to_json(smooth_tempogram)));
        smooth_tempogram_plot.charts.emplace_back("peak tempo", plt::arma_to_json(t), plt::arma_to_json(tempo_curve));
        viz_file.plots.push_back(smooth_tempogram_plot);

        std::vector<vec> pulses;
        for(const auto &multiple : settings.tempo_multiples) {
            vec pulse(novelty_curve.n_rows, fill::zeros);
            int window_length;
            int offset;
            uword start;
            for(const auto &section : tempo_sections) {
                window_length = static_cast<int>(round((section.end - section.start) * feature_rate));
                offset = static_cast<int>(round((section.start - section.offset) * feature_rate));
                start = (uword)(section.start * feature_rate);
                auto tmpuls = signal_utils::generate_pulse(section.bpm * multiple, window_length, feature_rate, offset);
                pulse(span(start, start + window_length - 1)) = std::get<0>(tmpuls);
            }
            pulses.push_back(pulse);
        }

        double mag = max(novelty_curve) * 0.2;

        plt::Plot nc_plot("Novelty curve & Computed BPM Onsets");
        vec t_nc = regspace<vec>(0, novelty_curve.n_rows) / feature_rate;
        nc_plot.charts.emplace_back("novelty curve", plt::arma_to_json(t_nc), plt::arma_to_json(novelty_curve));
        for(int i = 0; i < settings.tempo_multiples.size(); ++i) {
            vec y = pulses[i] % (pulses[i] > 0) * mag;
            nc_plot.charts.emplace_back("1/" + std::to_string(settings.tempo_multiples[i]) + " notes",
                    plt::arma_to_json(t_nc), plt::arma_to_json(y));
        }
        viz_file.plots.push_back(nc_plot);

        plt::Plot diff_plot("Overlap Novelty curve & Computed BPM Onsets");
        for(int i = 0; i < settings.tempo_multiples.size(); ++i) {
            vec y = pulses[i] % novelty_curve;
            diff_plot.charts.emplace_back("1/" + std::to_string(settings.tempo_multiples[i]) + " notes",
                                        plt::arma_to_json(t_nc), plt::arma_to_json(y));
        }
        viz_file.plots.push_back(diff_plot);


        viz_file.save(base_file + ".html");
    }

    if (settings.dump_data) {
        std::string base_dir = base_file + "/";
        fs::create_directories(base_dir.c_str());

        write_matrix_data(base_dir + "novelty_curve.npd", novelty_curve, (char) (TYPE_DOUBLE),
                          (char *) &feature_rate, sizeof(feature_rate));
        write_matrix_data(base_dir + "tempogram.npd", tempogram, (char) (TYPE_DOUBLE | TYPE_COMPLEX));
        write_matrix_data(base_dir + "t.npd", t, (char) (TYPE_DOUBLE));
        write_matrix_data(base_dir + "bpm.npd", bpm, (char) (TYPE_DOUBLE));
        write_matrix_data(base_dir + "tempogram_cyclic.npd", cyclic_tempgram, (char) (TYPE_DOUBLE),
                          (char *) &ref_tempo, sizeof(ref_tempo));
        write_matrix_data(base_dir + "ct_y_axis.npd", ct_y_axis, (char) (TYPE_DOUBLE));
        write_matrix_data(base_dir + "smooth_tempogram.npd", smooth_tempogram, (char) (TYPE_DOUBLE),
                          (char *) &smooth_length_samples, sizeof(smooth_length_samples));
        write_matrix_data(base_dir + "t_smooth.npd", t, (char) (TYPE_DOUBLE));
        write_matrix_data(base_dir + "tempo_curve.npd", tempo_curve, (char) (TYPE_DOUBLE),
                          (char *) &settings.min_section_length, sizeof(settings.min_section_length));

        write_sections(base_dir + "sections.txt", tempo_sections);
    }

    if (settings.format_for_osu) {
        std::cout << std::endl << "Osu timing points" << std::endl;
        for (const auto &section : tempo_sections) std::cout << section_to_osu(section) << std::endl;
    }
}