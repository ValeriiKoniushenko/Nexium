/*
 * MIT License
 *
 * Copyright (c) 2018-2026 Valerii Koniushenko
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

#include "Misc/BaseLog.h"
#include "Misc/Configs.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace Core
{

    void CacheSystem::write(const IDataIO& data, const nlohmann::json& json)
    {
        if (!createCacheDirIfNotExist(data))
        {
            return;
        }

        const auto str = json.dump(4);

        std::ofstream ofs(getPath(data));
        if (!ofs)
        {
            errorLog("Can't write cache for this object {}. Path: {}. Details: {}"_f
                     << data.getCacheHash() << getPath(data) << std::strerror(errno));
            return;
        }

        ofs.write(str.c_str(), static_cast<std::streamsize>(str.size()));
    }

    bool CacheSystem::hasCache(const IDataIO& data) const
    {
        return fs::exists(getPath(data));
    }

    void CacheSystem::clearCache(const IDataIO& data)
    {
        if (hasCache(data))
        {
            std::error_code ec;
            fs::remove(getPath(data), ec);
            if (ec)
            {
                errorLog("Can't clear cache for this object {}. Path: {}. Details: {}"_f
                         << data.getCacheHash() << getPath(data) << ec.message());
            }
        }
    }

    std::filesystem::path CacheSystem::getPath(const IDataIO& data) const
    {
        return getCachePath(data) / (data.getCacheHash().toStdString() + ".json");
    }

    std::filesystem::path CacheSystem::getCachePath(const IDataIO& data) const
    {
        return Config::Path::data / data.getCacheDir();
    }

    bool CacheSystem::createCacheDirIfNotExist(const IDataIO& data) const
    {
        auto cachePath = getCachePath(data);
        if (!std::filesystem::exists(cachePath))
        {
            std::error_code ec;
            std::filesystem::create_directories(cachePath, ec);
            if (ec)
            {
                errorLog("Can't create cache directory for this object {}. Path: {}. Details: {}"_f
                         << data.getCacheHash() << cachePath.generic_string() << ec.message());
                return false;
            }
        }
        return true;
    }

    spdlog::logger* CacheSystem::getLogger() const
    {
        static auto logger = spdlog::stdout_color_mt("CacheSystem");
        return logger.get();
    }

    DataStream::Result DataStream::field(IDataUpdateBridge& bridge)
    {
        if (_data->mode == Mode::Input)
        {
            if (!contains(bridge.getCacheHash().c_str()))
            {
                _data->errors.emplace_back(Result::ReadFailed, bridge.getCacheHash().c_str());
                return Result::ReadFailed;
            }
        }

        bridge.ioFieldsUpdate(*this);

        return Result::Success;
    }

    DataStream::Result DataStream::nesting(const char* key,
                                           const std::function<void(DataStream&)>& callback)
    {
        if (!key)
        {
            _data->errors.emplace_back(Result::InvalidPassedData, "nullptr");
            return Result::InvalidPassedData;
        }

        try
        {
            DataStream nestedStream;
            nestedStream.setMode(_data->mode);

            if (_data->mode == Mode::Input)
            {
                if (!contains(key))
                {
                    _data->errors.emplace_back(Result::ReadFailed, key);
                    return Result::ReadFailed;
                }

                nestedStream.getRaw() = finalJson()[key];
                callback(nestedStream);
            }
            else
            {
                callback(nestedStream);
                finalJson()[key] = nestedStream.getRaw();
            }
        }
        catch (const std::exception& er)
        {
            _data->errors.emplace_back(Result::CustomProcessingError, er.what());
            return Result::CustomProcessingError;
        }
        catch (...)
        {
            _data->errors.emplace_back(Result::CustomProcessingError, "Undefined internal error");
            return Result::CustomProcessingError;
        }

        return Result::Success;
    }

    DataStream::Result DataStream::array(
        const char* key, const std::function<void(DataStream&, std::size_t)>& callback)
    {
        if (!key)
        {
            _data->errors.emplace_back(Result::InvalidPassedData, "nullptr");
            return Result::InvalidPassedData;
        }

        try
        {
            DataStream nestedStream;
            nestedStream.setMode(_data->mode);

            if (_data->mode == Mode::Input)
            {
                if (!contains(key))
                {
                    _data->errors.emplace_back(Result::ReadFailed, key);
                    return Result::ReadFailed;
                }

                nestedStream.getRaw() = finalJson()[key];
                callback(nestedStream, finalJson()[key].size());
            }
            else
            {
                nestedStream.getRaw() = nlohmann::json::array();
                callback(nestedStream, 0);
                finalJson()[key] = nestedStream.getRaw();
            }
        }
        catch (const std::exception& er)
        {
            _data->errors.emplace_back(Result::CustomProcessingError, er.what());
            return Result::CustomProcessingError;
        }
        catch (...)
        {
            _data->errors.emplace_back(Result::CustomProcessingError, "Undefined internal error");
            return Result::CustomProcessingError;
        }

        return Result::Success;
    }

    bool DataStream::contains(const char* key) const
    {
        return finalJson().contains(key);
    }

    DataStream DataStream::dedicatedNesting(const char* key)
    {
        return DataStream(_data, key);
    }

    void DataStream::tryPushBackEmptyArrayElement()
    {
        if (_data->mode == Mode::Output)
        {
            auto& json = finalJson();
            if (json.is_array())
            {
                json.push_back(Json());
            }
        }
    }

    DataStream::DataStream(const IntrusivePtr<DataProvider>& viewing, const StringAtom& nesting)
    {
        _data = viewing;
        _extraNestingKey = nesting;
        _isViewer = true;
    }

    DataStream::Json& DataStream::finalJson()
    {
        if (_extraNestingKey.empty())
        {
            return _data->json;
        }

        Json* json = &_data->json;
        if (json->is_array())
        {
            json = &json->back();
        }

        if (!json->contains(_extraNestingKey))
        {
            (*json)[_extraNestingKey] = Json();
        }

        return (*json)[_extraNestingKey];
    }

    bool IDataUpdateBridge::Rules::checkField(const StringAtom& fieldName,
                                              uint32_t flag) const noexcept
    {
        const auto it = field.find(fieldName);
        if (it == field.end())
        {
            return false;
        }

        return (it->second & flag) != 0u;
    }

    bool IDataStreamBridge::hasCache() const
    {
        return fs::exists(getTargetCachePath());
    }

    void IDataStreamBridge::writeToCache()
    {
        if (auto dir = getCacheDir(); !dir.empty())
        {
            std::error_code ec;
            dir = getTargetCacheDirPath();

            fs::create_directories(dir, ec);
            if (!fs::exists(dir) || ec)
            {
                Assert(false);
                globalLog.errorLog(
                    "[Cache system] Can't create a dirs for cache for this object {}. Provided path: {}.{}"_f
                    << getCacheHash() << dir.generic_string()
                    << (ec ? " Details:" + ec.message() : ""));
                return;
            }
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
        return getTargetCacheDirPath() / (getCacheHash().toStdString() + ".json");
    }

    std::filesystem::path IDataStreamBridge::getTargetCacheDirPath() const
    {
        return Config::Path::config / getCacheDir();
    }

    DataStream::DataStream()
    {
        _data = new DataProvider();
    }

} // namespace Core