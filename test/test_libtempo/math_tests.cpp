//
// Created by egordm on 23-9-2018.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <math_utils.hpp>
#include "test_helpers.h"

namespace tempo_math = libtempo::utils::math;

class MathTests : public ::testing::Test {

};

TEST_F(MathTests, MathTests_Fix_Test) {
    std::vector<double> xs = {-3.6, -3.5, -3.4, -.5, 0, .5, 3.4, 3.5, 3.6};
    std::vector<int> ys = {-3, -3, -3, 0, 0, 0, 3, 3, 3};

    for (int i = 0; i < xs.size(); ++i) {
        ASSERT_EQ(tempo_math::fix(xs[i]), ys[i]);
    }
}

TEST_F(MathTests, MathTests_Sinc_Test) {
    vec test_eps = {EPSILON / 2.};
    ASSERT_TRUE(COMP_DBL(tempo_math::sinc_fac(test_eps)[0], 1));
    vec test_x = join_cols(regspace<vec>(-10, -1), regspace<vec>(1, 10));
    vec test_y = sin(test_x * M_PI) / (test_x * M_PI);
    vec test_p = tempo_math::sinc_fac(test_x);
    for (int i = 0; i < test_x.size(); ++i) {
        ASSERT_TRUE(COMP_DBL(test_p[i], test_y[i]));
    }
}

TEST_F(MathTests, MathTests_GCD_Test) {
    ASSERT_EQ(tempo_math::calc_gcd(1337, 42), 7);
}

TEST_F(MathTests, MathTests_HANN_Test) {
    vec y = {0., 0.11697778, 0.41317591, 0.75, 0.96984631, 0.96984631, 0.75, 0.41317591, 0.11697778, 0.};
    vec x = tempo_math::my_hanning(10);
    ASSERT_TRUE(compare_vec(x,y));
}