//
// Created by egordm on 10-9-2018.
//

#include "flag_logic.h"
#include <signal_utils.h>
#include <tempogram_utils.h>
#include <filesystem>
#include "present_utils.h"

namespace fs = std::filesystem;
using namespace present_utils;


void visualize(const std::string &filepath, Settings settings, const vec &novelty_curve, const mat &tempogram, const vec &t,
               const vec &bpm, const mat &cyclic_tempogram, const vec &tempo_curve, const vec &ct_y_axis,
               const mat &smooth_tempogram, int ref_tempo, int feature_rate,
               const std::vector<curve_utils::Section> &tempo_sections) {
    plt::File viz_file;

    std::vector<double> predicted_tempo;
    std::vector<double> predicted_tempo_rel;
    std::vector<double> predicted_tempo_t;
    for(const auto &section : tempo_sections) {
        predicted_tempo.push_back(section.bpm);
        predicted_tempo.push_back(section.bpm);
        predicted_tempo_rel.push_back(libtempo::tempogram_utils::bpm_to_cyclic(section.bpm, ref_tempo));
        predicted_tempo_rel.push_back(libtempo::tempogram_utils::bpm_to_cyclic(section.bpm, ref_tempo));
        predicted_tempo_t.push_back(section.start);
        predicted_tempo_t.push_back(section.end - 0.0001);
    }

    // Plot tempogram
    plt::Plot tempogram_plot("Tempogram");
    tempogram_plot.charts.push_back(plt::Chart::create_heatmap("tempogram", plt::arma_to_json(t),
                                                               plt::arma_to_json(bpm),
                                                               plt::arma_to_json(tempogram)));
    for (const auto &multiple : settings.tempo_multiples) {
        std::string label = multiple == 1 ? "Tempo curve ref" : "Tempo curve " + std::to_string(multiple) + "x";
        tempogram_plot.charts.emplace_back(label, plt::arma_to_json(t),
                                           plt::arma_to_json((vec) (tempo_curve * ref_tempo * multiple)));
    }
    tempogram_plot.charts.emplace_back("Predict bpm", predicted_tempo_t, predicted_tempo);
    tempogram_plot.attributes["xaxis"]["title"] = "Time (s)";
    tempogram_plot.attributes["xaxis"]["range"] = {tempo_sections.front().start, tempo_sections.back().end};
    tempogram_plot.attributes["yaxis"]["title"] = "Tempo (BPM)";
    viz_file.plots.push_back(tempogram_plot);

    // Plot Cyclic Tempogram
    plt::Plot cyclic_tempogram_plot("Cyclic Tempogram | Ref tempo = " + std::to_string(ref_tempo));
    cyclic_tempogram_plot.charts.push_back(plt::Chart::create_heatmap("tempogram", plt::arma_to_json(t),
                                                                      plt::arma_to_json(ct_y_axis),
                                                                      plt::arma_to_json(cyclic_tempogram)));
    cyclic_tempogram_plot.charts.emplace_back("peak tempo", plt::arma_to_json(t), plt::arma_to_json(tempo_curve));
    cyclic_tempogram_plot.charts.emplace_back("predicted tempo", predicted_tempo_t, predicted_tempo_rel);
    cyclic_tempogram_plot.attributes["xaxis"]["title"] = "Time (s)";
    cyclic_tempogram_plot.attributes["xaxis"]["range"] = {tempo_sections.front().start, tempo_sections.back().end};
    cyclic_tempogram_plot.attributes["yaxis"]["title"] = "Tempo (rBPM)";
    viz_file.plots.push_back(cyclic_tempogram_plot);

    // Plot Smoothed Tempogram
    plt::Plot smooth_tempogram_plot("Smoothed Tempogram | Smooth length = " + std::to_string(settings.smooth_length));
    smooth_tempogram_plot.charts.push_back(plt::Chart::create_heatmap("tempogram", plt::arma_to_json(t),
                                                                      plt::arma_to_json(ct_y_axis),
                                                                      plt::arma_to_json(smooth_tempogram)));
    smooth_tempogram_plot.charts.emplace_back("peak tempo", plt::arma_to_json(t), plt::arma_to_json(tempo_curve));
    smooth_tempogram_plot.charts.emplace_back("predicted tempo", predicted_tempo_t, predicted_tempo_rel);
    smooth_tempogram_plot.attributes["xaxis"]["title"] = "Time (s)";
    smooth_tempogram_plot.attributes["xaxis"]["range"] = {tempo_sections.front().start, tempo_sections.back().end};
    smooth_tempogram_plot.attributes["yaxis"]["title"] = "Tempo (rBPM)";
    viz_file.plots.push_back(smooth_tempogram_plot);

    // Generate pulses for bpm sections
    std::vector<vec> pulses;
    for (const auto &multiple : settings.tempo_multiples) {
        vec pulse(novelty_curve.n_rows, fill::zeros);
        int window_length;
        int offset;
        uword start;
        for (const auto &section : tempo_sections) {
            window_length = static_cast<int>(round((section.end - section.start) * feature_rate));
            offset = static_cast<int>(round((section.start - section.offset) * feature_rate));
            start = (uword) (section.start * feature_rate);
            auto tmpuls = signal_utils::generate_pulse(section.bpm * multiple, window_length, feature_rate, offset);
            pulse(span(start, start + window_length - 1)) = std::get<0>(tmpuls);
        }
        pulses.push_back(pulse);
    }

    double mag = max(novelty_curve) * 0.2;

    // Plot Novelty curve & pulses
    plt::Plot nc_plot("Novelty curve & Computed BPM Onsets");
    vec t_nc = regspace<vec>(0, novelty_curve.n_rows) / feature_rate;
    nc_plot.charts.emplace_back("novelty curve", plt::arma_to_json(t_nc), plt::arma_to_json(novelty_curve));
    for (int i = 0; i < settings.tempo_multiples.size(); ++i) {
        vec y = pulses[i] % (pulses[i] > 0) * mag;
        nc_plot.charts.emplace_back("1/" + std::to_string(settings.tempo_multiples[i] * 4) + " notes",
                                    plt::arma_to_json(t_nc), plt::arma_to_json(y));
    }
    nc_plot.attributes["xaxis"]["title"] = "Time (s)";
    nc_plot.attributes["yaxis"]["title"] = "Amplitude";
    viz_file.plots.push_back(nc_plot);

    // Plot diff
    plt::Plot diff_plot("Overlap Novelty curve & Computed BPM Onsets");
    for (int i = 0; i < settings.tempo_multiples.size(); ++i) {
        vec y = pulses[i] % novelty_curve;
        diff_plot.charts.emplace_back("1/" + std::to_string(settings.tempo_multiples[i] * 4) + " notes",
                                      plt::arma_to_json(t_nc), plt::arma_to_json(y));
    }
    diff_plot.attributes["xaxis"]["title"] = "Time (s)";
    diff_plot.attributes["yaxis"]["title"] = "Amplitude";
    viz_file.plots.push_back(diff_plot);


    viz_file.save(filepath + ".html");
}

void dump(const std::string &filepath, Settings settings, const vec &novelty_curve, const mat &tempogram, const vec &t,
          const vec &bpm, const mat &cyclic_tempogram, const vec &tempo_curve, const vec &ct_y_axis,
          const mat &smooth_tempogram, int ref_tempo, int feature_rate,
          const std::vector<curve_utils::Section> &tempo_sections) {

    std::string base_dir = filepath;
    fs::create_directories(base_dir.c_str());
    base_dir += "/";

    write_matrix_data(base_dir + "novelty_curve.npd", novelty_curve, (char) (TYPE_DOUBLE),
                      (char *) &feature_rate, sizeof(feature_rate));
    write_matrix_data(base_dir + "tempogram.npd", tempogram, (char) (TYPE_DOUBLE | TYPE_COMPLEX));
    write_matrix_data(base_dir + "t.npd", t, (char) (TYPE_DOUBLE));
    write_matrix_data(base_dir + "bpm.npd", bpm, (char) (TYPE_DOUBLE));
    write_matrix_data(base_dir + "tempogram_cyclic.npd", cyclic_tempogram, (char) (TYPE_DOUBLE),
                      (char *) &ref_tempo, sizeof(ref_tempo));
    write_matrix_data(base_dir + "ct_y_axis.npd", ct_y_axis, (char) (TYPE_DOUBLE));
    write_matrix_data(base_dir + "smooth_tempogram.npd", smooth_tempogram, (char) (TYPE_DOUBLE),
                      (char *) &settings.smooth_length, sizeof(settings.smooth_length));
    write_matrix_data(base_dir + "t_smooth.npd", t, (char) (TYPE_DOUBLE));
    write_matrix_data(base_dir + "tempo_curve.npd", tempo_curve, (char) (TYPE_DOUBLE),
                      (char *) &settings.min_section_length, sizeof(settings.min_section_length));

    write_sections(base_dir + "sections.txt", tempo_sections);

}
