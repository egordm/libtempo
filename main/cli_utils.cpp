//
// Created by egordm on 5-9-2018.
//

#include <iostream>
#include "cli_utils.h"
#include "settings.h"

void parse_arguments(Settings &settings, int argc, char **argv, bool &exit, bool &error) {
    ArgumentParser parser(
            "CLI for tempo estimation developed by Egor Dmitriev.\nVisit github for library version.");
    HelpFlag help(parser, "help", "Display the help menu", {'h', "help"});
    args::Group basic_args(parser, "Basic");

    ValueFlag<std::tuple<int, int>, IntsReader> bpm_scan_window_arg(parser, "bpm_window", BPM_SCAN_WINDOW_DESC, {"bpm_window"}, settings.bpm_scan_window);
    ValueFlagList<int> tempo_multiples_arg(parser, "tempo_multiples", TEMPO_MULTIPLES_DESC, {"tempo_multiples", 'm'},  settings.tempo_multiples);

    std::vector<Applyable*> arguments;
    arguments.push_back(new SettingArg<float>(basic_args, "bpm_rounding_precision", BPM_ROUNDING_PRECISION_DESC, {"bpm_rounding_precision"}, settings.bpm_rounding_precision));
    arguments.push_back(new SettingArg<int>(basic_args, "tempo_window", TEMPO_WINDOW_DESC, {"tempo_window"}, settings.tempo_window));
    arguments.push_back(new SettingArg<int>(basic_args, "ref_tempo", REF_TEMPO_DESC, {"ref_tempo"}, settings.ref_tempo));
    arguments.push_back(new SettingArg<int>(basic_args, "octave_divider", OCTAVE_DIVIDER_DESC, {"octave_divider"}, settings.octave_divider));
    arguments.push_back(new SettingArg<double>(basic_args, "smooth_length", SMOOTH_LENGTH_DESC, {"smooth_length"}, settings.smooth_length));
    arguments.push_back(new SettingArg<float>(basic_args, "triplet_weight", TRIPLET_WEIGH_DESC, {"triplet_weight"}, settings.triplet_weight));
    arguments.push_back(new SettingArg<double>(basic_args, "min_section_length", MIN_SECTION_LENGTH_DESC, {"min_section_length"}, settings.min_section_length));
    arguments.push_back(new SettingArg<double>(basic_args, "max_section_length", MAX_SECTION_LENGTH_DESC, {"max_section_length"}, settings.max_section_length));
    arguments.push_back(new SettingArg<float>(basic_args, "bpm_doubt_window", BPM_DOUBT_WINDOW_DESC, {"bpm_doubt_window"}, settings.bpm_doubt_window));
    arguments.push_back(new SettingArg<float>(basic_args, "bpm_doubt_step", BPM_DOUBT_STEP_DESC, {"bpm_doubt_step"}, settings.bpm_doubt_step));
    arguments.push_back(new SettingArg<bool>(basic_args, "generate_click_track", GENERATE_CLICK_TRACK_DESC, {"generate_click_track", 'c'}, settings.generate_click_track));
    arguments.push_back(new SettingArg<int>(basic_args, "click_track_subdivision", CLICK_TRACK_SUBDIVISION_DESC, {"click_track_subdivision"}, settings.click_track_subdivision));

    Flag osu_arg(parser, "osu", OSU_DESC, {"osu"});
    Flag viz_arg(parser, "viz", VIZ_DESC, {"viz"});
    Positional<std::string> audio_arg(parser, "audio", AUDIO_DESC);

    try {
        parser.ParseCLI(argc, argv);
    } catch (Help &) {
        std::cout << parser;
        exit = true;
        return;
    } catch (ParseError &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        exit = true;
        error = true;
        return;
    } catch (ValidationError &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        exit = true;
        error = true;
        return;
    }

    if(!audio_arg) {
        std::cerr << "Please specify a input file or use -h argument for help." << std::endl;
        exit = true;
        error = true;
        return;
    }
    settings.audio_file = get(audio_arg);

    for(Applyable* a : arguments) {
        a->apply();
        delete a;
    }
    arguments.clear();

    if(bpm_scan_window_arg) settings.bpm_scan_window = get(bpm_scan_window_arg);
    if(tempo_multiples_arg) settings.tempo_multiples = get(tempo_multiples_arg);
    if(osu_arg) settings.format_for_osu = true;
    if(viz_arg) settings.format_for_visualization = true;

    return;
}