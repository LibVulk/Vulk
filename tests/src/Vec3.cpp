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

#include <Vulk/Vec3.hpp>
#include <gtest/gtest.h>

vulk::Vec3 vc{3, -4, 1};
vulk::Vec3d vc2{5.1, 7.3, 4.2};
vulk::Vec3f vc3{2.f, 10.f, 6.f};
vulk::Vec3i vc4{-5, -2, -7};
vulk::Vec3u vc5{5, 12, 9};

TEST(Vec3Tests, InitTests)
{
    EXPECT_EQ(vc.x, 3);
    EXPECT_EQ(vc.y, -4);
    EXPECT_EQ(vc.z, 1);

    EXPECT_EQ(vc2.x, 5.1);
    EXPECT_EQ(vc2.y, 7.3);
    EXPECT_EQ(vc2.z, 4.2);

    EXPECT_EQ(vc3.x, 2.f);
    EXPECT_EQ(vc3.y, 10.f);
    EXPECT_EQ(vc3.z, 6.f);

    EXPECT_EQ(vc4.x, -5);
    EXPECT_EQ(vc4.y, -2);
    EXPECT_EQ(vc4.z, -7);

    EXPECT_EQ(vc5.x, 5);
    EXPECT_EQ(vc5.y, 12);
    EXPECT_EQ(vc5.z, 9);
}

TEST(Vec3Tests, NegativeTests)
{
    auto nv = -vc;
    auto nv2 = -vc2;
    auto nv3 = -vc3;
    auto nv4 = -vc4;
    auto nv5 = -vc5;

    EXPECT_EQ(nv.x, -3);
    EXPECT_EQ(nv.y, 4);
    EXPECT_EQ(nv.z, -1);

    EXPECT_EQ(nv2.x, -5.1);
    EXPECT_EQ(nv2.y, -7.3);
    EXPECT_EQ(nv2.z, -4.2);

    EXPECT_EQ(nv3.x, -2.f);
    EXPECT_EQ(nv3.y, -10.f);
    EXPECT_EQ(nv3.z, -6.f);

    EXPECT_EQ(nv4.x, 5);
    EXPECT_EQ(nv4.y, 2);
    EXPECT_EQ(nv4.z, 7);

    EXPECT_EQ(nv5.x, -5);
    EXPECT_EQ(nv5.y, -12);
    EXPECT_EQ(nv5.z, -9);
}

TEST(Vec3Tests, AdditionTests)
{
    auto av = vc + vulk::Vec3{6, -1, 2};
    auto av2 = vc2 + vulk::Vec3d{4.6, 3.6, 2.2};
    auto av3 = vc3 + vulk::Vec3f{2.4f, 10.3f, 4.6f};
    auto av4 = vc4 + vulk::Vec3i{-3, -2, -1};
    auto av5 = vc5 + vulk::Vec3u{10, 4, 9};

    EXPECT_EQ(av.x, 9);
    EXPECT_EQ(av.y, -5);
    EXPECT_EQ(av.z, 3);

    EXPECT_EQ(av2.x, 9.7);
    EXPECT_EQ(av2.y, 10.9);
    EXPECT_EQ(av2.z, 6.4);

    EXPECT_EQ(av3.x, 4.4f);
    EXPECT_EQ(av3.y, 20.3f);
    EXPECT_EQ(av3.z, 10.6f);

    EXPECT_EQ(av4.x, -8);
    EXPECT_EQ(av4.y, -4);
    EXPECT_EQ(av4.z, -8);

    EXPECT_EQ(av5.x, 15);
    EXPECT_EQ(av5.y, 16);
    EXPECT_EQ(av5.z, 18);
}

TEST(Vec3Tests, SubstractionTests)
{
    auto sv = vc - vulk::Vec3{6, -1, 4};
    auto sv2 = vc2 - vulk::Vec3d{4.6, 3.5, 10.5};
    auto sv3 = vc3 - vulk::Vec3f{2.f, 11.f, 8.f};
    auto sv4 = vc4 - vulk::Vec3i{-3, -2, -4};
    auto sv5 = vc5 - vulk::Vec3u{1, 4, 7};

    EXPECT_EQ(sv.x, -3);
    EXPECT_EQ(sv.y, -3);
    EXPECT_EQ(sv.z, -3);

    EXPECT_EQ(sv2.x, 0.5);
    EXPECT_EQ(sv2.y, 3.8);
    EXPECT_EQ(sv2.z, -6.3);

    EXPECT_EQ(sv3.x, 0);
    EXPECT_EQ(sv3.y, -1);
    EXPECT_EQ(sv3.z, -2);

    EXPECT_EQ(sv4.x, -2);
    EXPECT_EQ(sv4.y, 0);
    EXPECT_EQ(sv4.z, -3);

    EXPECT_EQ(sv5.x, 4);
    EXPECT_EQ(sv5.y, 8);
    EXPECT_EQ(sv5.z, 2);
}

TEST(Vec3Tests, MultiplicationTests)
{
    auto mv = vc * 2;
    auto mv2 = vc2 * -5.0;
    auto mv3 = vc3 * 3.f;
    auto mv4 = vc4 * 10;
    auto mv5 = vc5 * 7u;

    EXPECT_EQ(mv.x, 6);
    EXPECT_EQ(mv.y, -8);
    EXPECT_EQ(mv.z, 2);

    EXPECT_EQ(mv2.x, -25.5);
    EXPECT_EQ(mv2.y, -36.5);
    EXPECT_EQ(mv2.z, -21);

    EXPECT_EQ(mv3.x, 6.f);
    EXPECT_EQ(mv3.y, 30.f);
    EXPECT_EQ(mv3.z, 18.f);

    EXPECT_EQ(mv4.x, -50);
    EXPECT_EQ(mv4.y, -20);
    EXPECT_EQ(mv4.z, -70);

    EXPECT_EQ(mv5.x, 35);
    EXPECT_EQ(mv5.y, 84);
    EXPECT_EQ(mv5.z, 63);
}
