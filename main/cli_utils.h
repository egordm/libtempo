//
// Created by egordm on 5-9-2018.
//

#ifndef PROJECT_CLI_UTILS_H
#define PROJECT_CLI_UTILS_H

#include <tuple>
#include <vector>
#include <args.hxx>
#include "settings.h"

using namespace args;

struct IntsReader {
    void operator()(const std::string &name, const std::string &value, std::tuple<int, int> &destination) {
        size_t commapos = value.find(',');
        if (commapos == std::string::npos) return;

        std::string v1 = value.substr(0, commapos);
        std::string v2 = value.substr(commapos + 1, value.size() - commapos - 1);

        std::get<0>(destination) = std::stoi(v1);
        std::get<1>(destination) = std::stoi(v2);
    }
};

void parse_arguments(Settings &settings, int argc, char **argv, bool &exit, bool &error);

struct Applyable { // IDK
    virtual ~Applyable() = default;

    virtual void apply() = 0;
};

template<typename T>
struct SettingArg : Applyable {
    T &setting;
    ValueFlag<T> arg;

    SettingArg(Group &group, const std::string &name, const std::string &help, Matcher &&matcher, T &setting)
            : setting(setting), arg(group, name, "(Default: " + std::to_string(setting) + ") " + help, std::move(matcher), setting) {
    }

    inline void apply() override {
        if (arg) setting = get(arg);
    }
};


#endif //PROJECT_CLI_UTILS_H
