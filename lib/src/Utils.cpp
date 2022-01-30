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

#include "Vulk/Utils.hpp"

#include <fstream>
#include <vector>

std::vector<char> vulk::utils::fileToBinary(const char* filePath)
{
    std::ifstream file{filePath, std::ios::ate | std::ios::binary};

    if (!file.is_open())
        // TODO: indicate permission error, not found, ...
        throw std::runtime_error(std::string(filePath) + ": unable to open file");

    // TODO: there has to be a better way for this...
    const auto fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), static_cast<long>(fileSize));

    return buffer;
}
