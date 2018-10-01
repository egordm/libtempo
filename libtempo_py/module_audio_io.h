//
// Created by egordm on 21-9-2018.
//

#ifndef PROJECT_MODULE_AUDIO_IO_H
#define PROJECT_MODULE_AUDIO_IO_H

#include <pybind11/pybind11.h>
#include <audio_processing.h>

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
}

#endif //PROJECT_MODULE_AUDIO_IO_H
