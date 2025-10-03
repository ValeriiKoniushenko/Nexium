/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "BaseAsset.h"

#include "../Misc/Configs.h"

#include <Utils/Functions.h>
#include <fstream>

namespace Core
{

    void BaseAsset::attachAndReadFromFile(const std::filesystem::path& path)
    {
        _assetPath = path;

        onReadData(nlohmann::json::parse(Utils::GetTextFileContentAs<std::string>(path)));
    }

    void BaseAsset::writeToFile()
    {
        if (_assetPath.empty())
        {
            DEBUG_ASSERT(false, "File is not sync!");
            return;
        }

        const auto data = onWriteData().dump(4);
        std::ofstream out(_assetPath);
        if (!out.is_open())
        {
            criticalLog("Can't open file for write: {}"_f << _assetPath);
            return;
        }
        out.write(data.c_str(), static_cast<std::streamsize>(data.length()));
    }

} // namespace Core