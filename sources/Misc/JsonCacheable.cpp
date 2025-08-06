// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "JsonCacheable.h"

#include "BaseLog.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace SW
{

    void JsonCacheable::writeToCache() const
    {
        fs::create_directories(getCacheDir());
        if (!Verify(fs::exists(getCacheDir())))
        {
            globalLog.errorLog("Can't create a dirs for cache. Provided path: "
                               + getCacheDir().generic_string());
            return;
        }

        const std::string file = getCacheHash().toStdString() + ".json";
        const auto fullPath = getCacheDir() / file;

        const auto data = toCacheData().dump(4);
        std::ofstream out(fullPath);
        if (!out.is_open())
        {
            globalLog.errorLog("Can't open cache file for write: " + fullPath.generic_string());
            return;
        }

        out.write(data.c_str(), data.length());
    }

    bool JsonCacheable::hasCache() const
    {
        return fs::exists(getTargetPath());
    }

    void JsonCacheable::readFromCache()
    {
        std::ifstream ifs(getTargetPath());
        if (!ifs.is_open())
        {
            globalLog.errorLog("Can't open cache file for read: "
                               + getTargetPath().generic_string());
            return;
        }

        const auto json = nlohmann::json::parse(ifs);
        ifs.close();
        try
        {
            fromCacheData(json);
        }
        catch (std::exception& e)
        {
            globalLog.errorLog("Exception while reading of the cache file: '{}'. The reason: "_f
                               << getTargetPath().generic_string() << e.what());
        }
    }

    void JsonCacheable::tryReadFromCache()
    {
        if (hasCache())
        {
            readFromCache();
        }
    }

    std::filesystem::path JsonCacheable::getTargetPath() const
    {
        return getCacheDir() / (getCacheHash().toStdString() + ".json");
    }

} // namespace SW