//
// Created by egordm on 10-9-2018.
//

#ifndef PROJECT_FLAG_LOGIC_H
#define PROJECT_FLAG_LOGIC_H

#include <armadillo>
#include <string>
#include <array>
#include <curve_utils.h>
#include "settings.h"
#include "plotly.h"

using namespace arma;
namespace plt = plotly;
using namespace tempogram;

// TODO: this is a mess
void visualize(const std::string &filepath, Settings settings, const vec &novelty_curve, const mat &tempogram, const vec &t,
               const vec &bpm, const mat &cyclic_tempogram, const vec &tempo_curve, const vec &ct_y_axis,
               const mat &smooth_tempogram, int ref_tempo, int feature_rate,
               const std::vector<curve_utils::Section> &tempo_sections);


void dump(const std::string &filepath, Settings settings, const vec &novelty_curve, const mat &tempogram, const vec &t,
          const vec &bpm, const mat &cyclic_tempogram, const vec &tempo_curve, const vec &ct_y_axis,
          const mat &smooth_tempogram, int ref_tempo, int feature_rate,
          const std::vector<curve_utils::Section> &tempo_sections);


#endif //PROJECT_FLAG_LOGIC_H
