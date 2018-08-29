//
// Created by egordm on 29-8-2018.
//

#include "generic_algothms.h"

uword tempogram::generic_algorithms::find_nearest(const vec &a, double value) {
    return abs(a - value).index_min();
}
