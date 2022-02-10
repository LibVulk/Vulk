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

#pragma once

#define VULK_NO_COPY(ClassName)           \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;

#define VULK_NO_MOVE(ClassName)      \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete;

#define VULK_NO_MOVE_OR_COPY(ClassName) \
    VULK_NO_COPY(ClassName)             \
    VULK_NO_MOVE(ClassName)

#define VULK_DEFAULT_COPY(ClassName)       \
    ClassName(const ClassName&) = default; \
    ClassName& operator=(const ClassName&) = default;

#define VULK_DEFAULT_MOVE(ClassName)  \
    ClassName(ClassName&&) = default; \
    ClassName& operator=(ClassName&&) = default;

#define VULK_DEFAULT_MOVE_AND_COPY(ClassName) \
    VULK_DEFAULT_COPY(ClassName)              \
    VULK_DEFAULT_MOVE(ClassName)

/**
 * Forces a compile error and shows the type of a variable.
 * Useful tool when searching for an auto type or deep library calls.
 * Inspired from Scott Meyers' Effective C++ book.
 * @tparam T Type you are looking for, use decltype of the variable to detect it.
 */
template<typename T>
class TypeChecker;
