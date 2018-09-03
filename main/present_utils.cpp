//
// Created by egordm on 3-9-2018.
//

#include "present_utils.h"

std::string present_utils::section_to_osu(const curve_utils::Section &section) {
    std::stringstream ss;
    ss << (uword) (section.offset * 1000) << ","
       << (uword) (60000 / section.bpm) << ","
       << "4,1,0,100,0,1";

    return ss.str();
}
