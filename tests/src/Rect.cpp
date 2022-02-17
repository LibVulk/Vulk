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

#include <Vulk/Rect.hpp>
#include <gtest/gtest.h>

vulk::Rect r{0, 5, 10, 5};
vulk::Rectf r2{1.5f, -4.f, 2.f, 6.5f};
vulk::Rectd r3{-2.5, 5.2, 10.5, 10.5};
vulk::Recti r4{-2, -1, 5, 12};

TEST(RectTests, InitTests)
{
    EXPECT_EQ(r.left, 0);
    EXPECT_EQ(r.top, 5);
    EXPECT_EQ(r.width, 10);
    EXPECT_EQ(r.height, 5);

    EXPECT_EQ(r2.left, 1.5);
    EXPECT_EQ(r2.top, -4);
    EXPECT_EQ(r2.width, 2);
    EXPECT_EQ(r2.height, 6.5);

    EXPECT_EQ(r3.left, -2.5);
    EXPECT_EQ(r3.top, 5.2);
    EXPECT_EQ(r3.width, 10.5);
    EXPECT_EQ(r3.height, 10.5);

    EXPECT_EQ(r4.left, -2);
    EXPECT_EQ(r4.top, -1);
    EXPECT_EQ(r4.width, 5);
    EXPECT_EQ(r4.height, 12);
}

TEST(RectTests, EqualityTests)
{
    auto recTest = vulk::Rect{4, 5, 5, 24};

    EXPECT_EQ(r == recTest, false);
    EXPECT_EQ(r2 == r2, true);
    EXPECT_EQ(r3 == r3, true);
    EXPECT_EQ(r4 == r, false);
}

TEST(RectTests, InequalityTests)
{
    auto recTest = vulk::Rect{4, 5, 5, 24};

    EXPECT_EQ(r != recTest, true);
    EXPECT_EQ(r2 != r2, false);
    EXPECT_EQ(r3 != r3, false);
    EXPECT_EQ(r4 != r, true);
}

TEST(RectTests, ContainsTests)
{
    EXPECT_EQ(r.contains(2, 2), false);
    EXPECT_EQ(r2.contains(1.f, 10.f), false);
    EXPECT_EQ(r3.contains(3, 12), true);
    EXPECT_EQ(r4.contains(-1, 2), true);
}
