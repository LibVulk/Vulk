/* 
**   Copyright 2021 Maxime Houis
**
**   Licensed under the Apache License, Version 2.0 (the "License");
**   you may not use this file except in compliance with the License.
**   You may obtain a copy of the License at
**
**       http://www.apache.org/licenses/LICENSE-2.0
**
**   Unless required by applicable law or agreed to in writing, software
**   distributed under the License is distributed on an "AS IS" BASIS,
**   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**   See the License for the specific language governing permissions and
**   limitations under the License.
*/

#pragma once

#include <vector>

namespace vulk::utils {
template<typename T, typename Allocator>
[[nodiscard]] bool vectorContains(const std::vector<T, Allocator>& vector, const T& element)
{
    return std::find(vector.cbegin(), vector.cend(), element) != vector.cend();
}

std::vector<char> fileToBinary(const char* filePath);
}  // namespace vulk::utils
