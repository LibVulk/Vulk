/*
 * Copyright (c) 2021-2021 [fill name later]
 *
 * This software is provided "as-is", without any express or implied warranty. In no event
 *     will the authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including commercial
 *     applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim that you
 *     wrote the original software. If you use this software in a product, an acknowledgment
 *     in the product documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented
 * as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <Vulk/Mat3.hpp>
#include <gtest/gtest.h>

vulk::Mat3<double> m1{1.0};
vulk::Mat3<double> m2{1.0, 2.0, -1.0, 2.0, 1.0, -1.0, -1.0, 1.0, 2.0};

TEST(Mat3Tests, InitTests)
{
    EXPECT_EQ(m1.m[0], 1.0);
    EXPECT_EQ(m1.m[1], 0.0);
    EXPECT_EQ(m1.m[2], 0.0);
    EXPECT_EQ(m1.m[3], 0.0);
    EXPECT_EQ(m1.m[4], 1.0);
    EXPECT_EQ(m1.m[5], 0.0);
    EXPECT_EQ(m1.m[6], 0.0);
    EXPECT_EQ(m1.m[7], 0.0);
    EXPECT_EQ(m1.m[8], 1.0);

    EXPECT_EQ(m2.m[0], 1.0);
    EXPECT_EQ(m2.m[1], 2.0);
    EXPECT_EQ(m2.m[2], -1.0);
    EXPECT_EQ(m2.m[3], 2.0);
    EXPECT_EQ(m2.m[4], 1.0);
    EXPECT_EQ(m2.m[5], -1.0);
    EXPECT_EQ(m2.m[6], -1.0);
    EXPECT_EQ(m2.m[7], 1.0);
    EXPECT_EQ(m2.m[8], 2.0);
}

TEST(Mat3Tests, CoFactorTests)
{
    auto coFactor1 = m1.coFactor();
    auto coFactor2 = m2.coFactor();

    EXPECT_EQ(coFactor1.m[0], 1.0);
    EXPECT_EQ(coFactor1.m[1], 0.0);
    EXPECT_EQ(coFactor1.m[2], 0.0);
    EXPECT_EQ(coFactor1.m[3], 0.0);
    EXPECT_EQ(coFactor1.m[4], 1.0);
    EXPECT_EQ(coFactor1.m[5], 0.0);
    EXPECT_EQ(coFactor1.m[6], 0.0);
    EXPECT_EQ(coFactor1.m[7], 0.0);
    EXPECT_EQ(coFactor1.m[8], 1.0);

    EXPECT_EQ(coFactor2.m[0], 3.0);
    EXPECT_EQ(coFactor2.m[1], -3.0);
    EXPECT_EQ(coFactor2.m[2], 3.0);
    EXPECT_EQ(coFactor2.m[3], -5.0);
    EXPECT_EQ(coFactor2.m[4], 1.0);
    EXPECT_EQ(coFactor2.m[5], -3.0);
    EXPECT_EQ(coFactor2.m[6], -1.0);
    EXPECT_EQ(coFactor2.m[7], -1.0);
    EXPECT_EQ(coFactor2.m[8], -3.0);
}

TEST(Mat3Tests, DeterminantTests)
{
    auto d1 = m1.determinant();
    auto d2 = m2.determinant();

    EXPECT_EQ(d1, 1);
    EXPECT_EQ(d2, -6.0);
}

TEST(Mat3Tests, InverseTests)
{
    auto i1 = m1.inverse();
    auto i2 = m2.inverse();

    EXPECT_EQ(i1.m[0], 1.0);
    EXPECT_EQ(i1.m[1], 0.0);
    EXPECT_EQ(i1.m[2], 0.0);
    EXPECT_EQ(i1.m[3], 0.0);
    EXPECT_EQ(i1.m[4], 1.0);
    EXPECT_EQ(i1.m[5], 0.0);
    EXPECT_EQ(i1.m[6], 0.0);
    EXPECT_EQ(i1.m[7], 0.0);
    EXPECT_EQ(i1.m[8], 1.0);

    EXPECT_NEAR(i2.m[0], -0.5, 0.01);
    EXPECT_NEAR(i2.m[1], 0.5, 0.01);
    EXPECT_NEAR(i2.m[2], -0.5, 0.01);
    EXPECT_NEAR(i2.m[3], 0.83, 0.01);
    EXPECT_NEAR(i2.m[4], -0.16, 0.01);
    EXPECT_NEAR(i2.m[5], 0.5, 0.01);
    EXPECT_NEAR(i2.m[6], 0.16, 0.01);
    EXPECT_NEAR(i2.m[7], 0.16, 0.01);
    EXPECT_NEAR(i2.m[8], 0.5, 0.01);
}
