//
// Created by egordm on 21-9-2018.
//

#ifndef PROJECT_MODULE_AUDIO_IO_H
#define PROJECT_MODULE_AUDIO_IO_H

#include <pybind11/pybind11.h>
#include <audio_processing.h>
#include <audio_annotation.h>

using namespace libtempo;
namespace py = pybind11;

void register_audio_io(pybind11::module &m) {
    py::class_<audio::AudioFile>(m, "AudioFile")
            .def_readwrite("path", &audio::AudioFile::path, R"pbdoc(Audio file path)pbdoc")
            .def_readwrite("data", &audio::AudioFile::data, R"pbdoc(Audio file data)pbdoc")
            .def_readwrite("sr", &audio::AudioFile::sr, R"pbdoc(Audio file sample rate)pbdoc")
            .def_readwrite("format", &audio::AudioFile::format, R"pbdoc(Audio file format mask)pbdoc")
            .def_static("open", &audio::AudioFile::open, py::arg("path"), R"pbdoc(Opens audiofile from given path)pbdoc")
            .def("save", &audio::AudioFile::save, py::arg("path"), R"pbdoc(Saves audio to a given path)pbdoc");

    auto ma = m.def_submodule("annotation");

    ma.def("generate_click", &audio::annotation::generate_click,
           R"pbdoc(
          Generates a click sound

          Args:
              sr: sample rate
              duration: click duration in seconds
              freq: click frequency

          Returns:
              click array

          )pbdoc",
           py::arg("sr") = 22050,
           py::arg("duration") = 0.1,
           py::arg("freq") = 1000.0
    );

    ma.def("click_track_from_positions", &audio::annotation::click_track_from_positions,
           R"pbdoc(
          Generates click track of given length for given click positions in seconds

          Args:
              positions: click positions in seconds
              length: length of the track in seconds
              sr: sample rate

          Returns:
              click track

          )pbdoc",
           py::arg("positions"),
           py::arg("length"),
           py::arg("sr") = 22050
    );

    ma.def("click_track_from_tempo", &audio::annotation::click_track_from_tempo,
           R"pbdoc(
          Generates a clicktrack by placing click on given notes of given bpm and offset.

          Args:
              bpm: click bpm
              offset: click offset
              length: length of the track in seconds
              note_fraction: note fraction which to place clicks on
              sr: sample rate

          Returns:
              click track

          )pbdoc",
           py::arg("bpm"),
           py::arg("offset"),
           py::arg("length"),
           py::arg("note_fraction") = 4,
           py::arg("sr") = 22050
    );
}

#endif //PROJECT_MODULE_AUDIO_IO_H
