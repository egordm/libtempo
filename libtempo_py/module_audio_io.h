//
// Created by egordm on 21-9-2018.
//

#ifndef PROJECT_MODULE_AUDIO_IO_H
#define PROJECT_MODULE_AUDIO_IO_H

#include <pybind11/pybind11.h>
#include "wrapper_audio_io.hpp"


void register_audio_io(pybind11::module &m) {
    py::class_<wrapper_audio_io::AudioWrapper>(m, "AudioWrapper")
            .def(py::init<const char *>(), py::arg("path"))
            .def("get_path", &wrapper_audio_io::AudioWrapper::get_path,
                 R"pbdoc(Gets path of the audio file)pbdoc")
            .def("get_sr", &wrapper_audio_io::AudioWrapper::get_sr,
                 R"pbdoc(Gets audio sample rate)pbdoc")
            .def("get_format", &wrapper_audio_io::AudioWrapper::get_format,
                 R"pbdoc(Gets audio format as a bitmask)pbdoc")
            .def("get_data", &wrapper_audio_io::AudioWrapper::get_data,
                 R"pbdoc(Gets audio data as a matrix wrapper)pbdoc")
            .def("set_data", &wrapper_audio_io::AudioWrapper::set_data,
                 R"pbdoc(Sets audio data with a matrix wrapper)pbdoc",
                 py::arg("data"))
            .def("save", &wrapper_audio_io::AudioWrapper::save,
                 R"pbdoc(Saves audio to a given path)pbdoc",
                 py::arg("path"));

}

#endif //PROJECT_MODULE_AUDIO_IO_H
