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

#include <Vulk/Color.hpp>
#include <gtest/gtest.h>

TEST(ColorTests, ConstructorsRGBA)
{
    EXPECT_EQ(vulk::Color().value(), 0);
    EXPECT_EQ(vulk::Color(0, 0, 0, 0).value(), 0x00000000);
    EXPECT_EQ(vulk::Color(255, 0, 0, 0).value(), 0xff000000);
    EXPECT_EQ(vulk::Color(0, 255, 0, 0).value(), 0x00ff0000);
    EXPECT_EQ(vulk::Color(0, 0, 255, 0).value(), 0x0000ff00);
    EXPECT_EQ(vulk::Color(0, 0, 0, 255).value(), 0x000000ff);
    EXPECT_EQ(vulk::Color(255, 202, 0, 255).value(), 0xffca00ff);
    EXPECT_EQ(vulk::Color(255, 202, 0, 0).value(), 0xffca0000);
    EXPECT_EQ(vulk::Color(255, 255, 255, 255).value(), 0xffffffff);
    EXPECT_EQ(vulk::Color(42, 84, 21, 255).value(), 0x2a5415ff);
}

TEST(ColorTests, ConstructorValue)
{
    EXPECT_EQ(vulk::Color{0x00000000}.value(), 0x00000000);
    EXPECT_EQ(vulk::Color{0xff000000}.value(), 0xff000000);
    EXPECT_EQ(vulk::Color{0x00ff0000}.value(), 0x00ff0000);
    EXPECT_EQ(vulk::Color{0x0000ff00}.value(), 0x0000ff00);
    EXPECT_EQ(vulk::Color{0x000000ff}.value(), 0x000000ff);
    EXPECT_EQ(vulk::Color{0xff0000ff}.value(), 0xff0000ff);
    EXPECT_EQ(vulk::Color{0xdeadbeef}.value(), 0xdeadbeef);
    EXPECT_EQ(vulk::Color{0x12345678}.value(), 0x12345678);
    EXPECT_EQ(vulk::Color{0xffffffff}.value(), 0xffffffff);
}

TEST(ColorTests, Equality)
{
    EXPECT_EQ(vulk::Color(255, 255, 255, 255), vulk::Color(0xffffffff));
    EXPECT_EQ(vulk::Color(255, 255, 255, 255), vulk::Color(255, 255, 255, 255));
    EXPECT_EQ(vulk::Color(0, 255, 0, 255), vulk::Color(0, 255, 0, 255));
    EXPECT_NE(vulk::Color(0, 255, 0, 255), vulk::Color(0, 255, 0, 0));
}
