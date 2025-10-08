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

#include "DataStream.h"

#include "BaseLog.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace Core
{

    DataStream::Result DataStream::nesting(const char* key,
                                           const std::function<void(DataStream&)>& callback)
    {
        if (!key)
        {
            _errors.emplace_back(Result::InvalidPassedData, "nullptr");
            return Result::InvalidPassedData;
        }

        try
        {
            DataStream nestedStream;
            nestedStream.setMode(_mode);

            if (_mode == Mode::Input)
            {
                if (!contains(key))
                {
                    _errors.emplace_back(Result::ReadFailed, key);
                    return Result::ReadFailed;
                }

                nestedStream.getRaw() = _json[key];
                callback(nestedStream);
            }
            else
            {
                callback(nestedStream);
                _json[key] = nestedStream.getRaw();
            }
        }
        catch (const std::exception& er)
        {
            _errors.emplace_back(Result::CustomProcessingError, er.what());
            return Result::CustomProcessingError;
        }
        catch (...)
        {
            _errors.emplace_back(Result::CustomProcessingError, "Undefined internal error");
            return Result::CustomProcessingError;
        }

        return Result::Success;
    }

    bool DataStream::contains(const char* key) const
    {
        return _json.contains(key);
    }

    bool IDataStreamBridge::hasCache() const
    {
        return fs::exists(getTargetCachePath());
    }

    void IDataStreamBridge::writeToCache()
    {
        std::error_code ec;
        fs::create_directories(getCacheDir(), ec);
        if (!fs::exists(getCacheDir()) || ec)
        {
            DEBUG_ASSERT(false);
            globalLog.errorLog(
                "[Cache system] Can't create a dirs for cache for this object {}. Provided path: {}.{}"_f
                << getCacheHash() << getCacheDir().generic_string()
                << (ec ? " Details:" + ec.message() : ""));
            return;
        }

        const auto fullPath = getTargetCachePath();

        DataStream stream;
        stream.setMode(DataStream::Mode::Output);
        ioFieldsUpdate(stream);
        const auto data = stream.getRaw().dump(4);
        std::ofstream out(fullPath);
        if (!out.is_open())
        {
            globalLog.errorLog(
                "[Cache system] Can't open cache file for write for this object {}. Details: {}"_f
                << getCacheHash() << fullPath.generic_string());
            return;
        }

        out.write(data.c_str(), static_cast<std::streamsize>(data.length()));
    }

    void IDataStreamBridge::readFromCache()
    {
        const auto targetPath = getTargetCachePath();
        DataStream stream;
        std::ifstream ifs(targetPath);
        if (!ifs.is_open())
        {
            globalLog.warnLog(
                "[Cache system] Can't open cache file for read for this object {}. Details: "_f
                << getCacheHash() << targetPath.generic_string());
            return;
        }

        stream.getRaw() = nlohmann::json::parse(ifs);
        ifs.close();

        try
        {
            stream.setMode(DataStream::Mode::Input);
            ioFieldsUpdate(stream);
        }
        catch (const std::exception& e)
        {
            globalLog.errorLog(
                "[Cache system] Exception while reading of the cache file: '{}' for this object {}. The reason: {}"_f
                << targetPath.generic_string() << getCacheHash() << e.what());
            clearCache();
        }
        catch (...)
        {
            globalLog.errorLog(
                "[Cache system] Exception while reading of the cache file: '{}' for this object{}."_f
                << targetPath.generic_string() << getCacheHash());
            clearCache();
        }
    }

    void IDataStreamBridge::tryReadFromCache()
    {
        if (hasCache())
        {
            readFromCache();
        }
    }

    void IDataStreamBridge::clearCache()
    {
        std::error_code ec;
        std::filesystem::remove_all(getTargetCachePath(), ec);
        if (ec)
        {
            globalLog.errorLog("[Cache system] Can't clear cache for this object {}. Details: {}"_f
                               << ec.message() << getCacheHash());
        }
    }

    std::filesystem::path IDataStreamBridge::getTargetCachePath() const
    {
        return getCacheDir() / (getCacheHash().toStdString() + ".json");
    }

} // namespace Core