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

#include <Vulk/Vec2.hpp>
#include <gtest/gtest.h>

static constexpr vulk::Vec2 v{3, -4};
static constexpr vulk::Vec2d v2{5.1, 7.3};
static constexpr vulk::Vec2f v3{2.f, 10.f};
static constexpr vulk::Vec2i v4{-5, -2};

TEST(Vec2Tests, InitTests)
{
    EXPECT_EQ(v.x, 3);
    EXPECT_EQ(v.y, -4);
    EXPECT_NEAR(v2.x, 5.1, 0.1);
    EXPECT_NEAR(v2.y, 7.3, 0.1);
    EXPECT_NEAR(v3.x, 2.f, 0.1);
    EXPECT_NEAR(v3.y, 10.f, 0.1);
    EXPECT_EQ(v4.x, -5);
    EXPECT_EQ(v4.y, -2);
}

TEST(Vec2Tests, NegativeTests)
{
    auto nv = -v;
    auto nv2 = -v2;
    auto nv3 = -v3;
    auto nv4 = -v4;

    EXPECT_EQ(nv.x, -3);
    EXPECT_EQ(nv.y, 4);
    EXPECT_NEAR(nv2.x, -5.1, 0.1);
    EXPECT_NEAR(nv2.y, -7.3, 0.1);
    EXPECT_NEAR(nv3.x, -2.f, 0.1);
    EXPECT_NEAR(nv3.y, -10.f, 0.1);
    EXPECT_EQ(nv4.x, 5);
    EXPECT_EQ(nv4.y, 2);
}

TEST(Vec2Tests, AdditionTests)
{
    auto av = v + vulk::Vec2{6, -1};
    auto av2 = v2 + vulk::Vec2d{4.6, 3.6};
    auto av3 = v3 + vulk::Vec2f{2.4f, 10.3f};
    auto av4 = v4 + vulk::Vec2i{-3, -2};

    EXPECT_EQ(av.x, 9);
    EXPECT_EQ(av.y, -5);
    EXPECT_NEAR(av2.x, 9.7, 0.1);
    EXPECT_NEAR(av2.y, 10.9, 0.1);
    EXPECT_NEAR(av3.x, 4.4f, 0.1);
    EXPECT_NEAR(av3.y, 20.3f, 0.1);
    EXPECT_EQ(av4.x, -8);
    EXPECT_EQ(av4.y, -4);
}

TEST(Vec2Tests, SubstractionTests)
{
    auto sv = v - vulk::Vec2{6, -1};
    auto sv2 = v2 - vulk::Vec2d{4.6, 3.5};
    auto sv3 = v3 - vulk::Vec2f{2.f, 11.f};
    auto sv4 = v4 - vulk::Vec2i{-3, -2};

    EXPECT_EQ(sv.x, -3);
    EXPECT_EQ(sv.y, -3);
    EXPECT_NEAR(sv2.x, 0.5, 0.1);
    EXPECT_NEAR(sv2.y, 3.8, 0.1);
    EXPECT_NEAR(sv3.x, 0, 0.1);
    EXPECT_NEAR(sv3.y, -1, 0.1);
    EXPECT_EQ(sv4.x, -2);
    EXPECT_EQ(sv4.y, 0);
}

TEST(Vec2Tests, MultiplicationTests)
{
    auto mv = v * 2;
    auto mv2 = v2 * -5.0;
    auto mv3 = v3 * 3.f;
    auto mv4 = v4 * 10;

    EXPECT_EQ(mv.x, 6);
    EXPECT_EQ(mv.y, -8);
    EXPECT_NEAR(mv2.x, -25.5, 0.1);
    EXPECT_NEAR(mv2.y, -36.5, 0.1);
    EXPECT_NEAR(mv3.x, 6.f, 0.1);
    EXPECT_NEAR(mv3.y, 30.f, 0.1);
    EXPECT_EQ(mv4.x, -50);
    EXPECT_EQ(mv4.y, -20);
}

TEST(Vec2Tests, DotTests)
{
    EXPECT_EQ(vulk::Vec2<int>::dot(v, v), 25);
    EXPECT_NEAR(vulk::Vec2d::dot(v2, v2), 79.3, 0.1);
    EXPECT_NEAR(vulk::Vec2f::dot(v3, v3), 104.f, 0.1);
    EXPECT_EQ(vulk::Vec2i::dot(v4, v4), 29);
}

TEST(Vec2Tests, CrossTests)
{
    EXPECT_EQ(vulk::Vec2<int>::cross(v, vulk::Vec2{-4, 3}), -7);
    EXPECT_NEAR(vulk::Vec2d::cross(v2, vulk::Vec2d{7.3, 5.1}), -27.28, 0.1);
    EXPECT_NEAR(vulk::Vec2f::cross(v3, vulk::Vec2f{10.f, 2.f}), -96.f, 0.1);
    EXPECT_EQ(vulk::Vec2i::cross(v4, vulk::Vec2i{-2, -5}), 21);
}

TEST(Vec2Tests, lengthSquaredTests)
{
    EXPECT_EQ(v.lengthSquared(), 25);
    EXPECT_NEAR(v2.lengthSquared(), 79.3, 0.1);
    EXPECT_NEAR(v3.lengthSquared(), 104.f, 0.1);
    EXPECT_EQ(v4.lengthSquared(), 29);
}

TEST(Vec2Tests, lengthTests)
{
    EXPECT_EQ(v.length(), 5);
    EXPECT_NEAR(v2.length(), 8.9, 0.1);
    EXPECT_NEAR(v3.length(), 10.1f, 0.1);
    EXPECT_EQ(v4.length(), 5);
}
