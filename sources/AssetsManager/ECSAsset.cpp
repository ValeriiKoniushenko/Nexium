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

#include "ECSAsset.h"

#include "AssetImpls/Factory.h"
#include "ModuleInfo.h"

#include <Utils/Functions.h>
#include <nlohmann/json.hpp>

namespace Core
{

    ECSAsset::~ECSAsset()
    {
        localClear();
    }

    spdlog::logger* ECSAsset::getLogger() const
    {
        return ::AssetsManager::getLogger();
    }

    void ECSAsset::connectSourceFile(const std::filesystem::path& src)
    {
        _pathToSource = src;

        if (_pathToSource.empty())
        {
            criticalLog("Empty path was passed to asset's sources");
            return;
        }

        if (!std::filesystem::exists(_pathToSource))
        {
            criticalLog("Asset's source file doesn't exist: {}"_f << _pathToSource);
            _pathToSource.clear();
            return;
        }

        extrudeAndValidateMainDataFromFile();
    }
    const std::filesystem::path& ECSAsset::getSourceFile() const noexcept
    {
        return _pathToSource;
    }

    nlohmann::json ECSAsset::getAssetData() const
    {
        try
        {
            const auto json = nlohmann::json::parse(Utils::GetFileContent(_pathToSource));

            if (!json.contains(AssetData::assetData))
            {
                return {};
            }

            return json[AssetData::assetData];
        }
        catch (const std::exception& e)
        {
            criticalLog("Can't provide asset's data: {}. Reason: {}"_f << _pathToSource
                                                                       << e.what());
        }
        catch (...)
        {
            criticalLog("Can't provide asset's data: {}. Due to internal error."_f
                        << _pathToSource);
        }

        return {};
    }

    void ECSAsset::syncAssetWithMemory(const nlohmann::json& assetData)
    {
        if (!Verify(_data))
        {
            criticalLog("Can't sync asset with memory. Asset is not loaded properly: data is null");
            return;
        }

        if (_status.cast() != Status::Loaded && _status.cast() != Status::PreLoaded)
        {
            warnLog("Can't sync asset with memory. Asset is not loaded properly.");
            return;
        }

        nlohmann::json json;
        json[AssetData::type] = _type;
        json[AssetData::name] = _name;
        json[AssetData::data] = nlohmann::json::object();

        json[AssetData::assetData] = assetData;

        if (_status.cast() == Status::Loaded)
        {
            DataStream stream;
            stream.setMode(DataStream::Mode::Output);
            _data->ioFieldsUpdate(stream);

            json[AssetData::name] = _data->getComponentName();
            json[AssetData::data] = std::move(stream.getRaw());
        }

        std::fstream out(_pathToSource, std::ios::out);
        if (out.is_open())
        {
            out << json.dump(4);
            traceLog("Asset: {} was updated successfully"_f << _logicPath);
        }
        else
        {
            criticalLog("Can't open file for write: {} - to update Asset:: {}"_f << _pathToSource
                                                                                 << _logicPath);
        }
    }

    bool ECSAsset::canProcessAction(AssetAction action) const
    {
        if (!_impl)
        {
            return false;
        }

        return _impl->canProcessAction(action);
    }

    void ECSAsset::processAction(AssetAction action)
    {
        if (!_impl || !canProcessAction(action))
        {
            return;
        }

        _impl->processAction(action);
    }

    void ECSAsset::load()
    {
        if (_status.cast() != Status::PreLoaded)
        {
            warnLog("Can't load asset: '{}'. Status is not 'PreLoaded'."_f << _logicPath);
            return;
        }

        try
        {
            traceLog("Loading of asset: {}"_f << _logicPath);

            _data.reset();
            _data = GetGlobalComponentFactory().create(_type);
            if (!_data)
            {
                throw std::runtime_error(
                    "Can't parse & recreate RTTI type. Maybe, the ECS type was not registered, or "
                    "type is template(it can cause problems).");
            }

            // Fetching main asset's data
            const auto json
                = nlohmann::json::parse(Utils::GetTextFileContentAs<std::string>(_pathToSource));

            // updating object's fields
            DataStream stream;
            stream.setMode(DataStream::Mode::Input);
            stream.getRaw() = json[AssetData::data];
            _data->ioFieldsUpdate(stream);

            // [opt] making loading of essential data (texture loading, 3D model loading, etc)
            if (_impl)
            {
                _impl->load(*this, _data.get(), json[AssetData::assetData]);
            }

            _status = Status::Loaded;
            traceLog("Asset:: '{}' is: loaded! New status is: 'Loaded'"_f << _logicPath);
        }
        catch (const std::exception& e)
        {
            criticalLog("Can't load Asset:: '{}'. The reason: {}. New status is: 'LoadingError'"_f
                        << _logicPath << e.what());
            _status = Status::LoadingError;
        }
        catch (...)
        {
            criticalLog(
                "Can't load Asset:: '{}'. The reason is undefined. New status is: 'LoadingError'"_f
                << _logicPath);
            _status = Status::LoadingError;
        }
    }

    void ECSAsset::unload()
    {
        _status = Status::PreLoaded;

        if (_impl)
        {
            _impl->unload(*this, _data.get());
        }
        _data.reset();

        traceLog("Asset: '{}' is unloaded its main data. New status is: 'PreLoaded'"_f
                 << _logicPath);
    }

    void ECSAsset::onIncrementRef(uint32_t count)
    {
        IntrusiveRefCounter<ECSAsset>::onIncrementRef(count);

        // why 2? The first ref is placing at AssetManager like some fake asset
        // we just know that it was indexed by the system.
        // With the second ref - the final code wants to use it. So, we must
        // load it.
        if (count == 2)
        {
            load();
        }
    }

    void ECSAsset::onDecrementRef(uint32_t count)
    {
        IntrusiveRefCounter<ECSAsset>::onDecrementRef(count);

        if (count == 1)
        {
            unload();
        }
    }

    void ECSAsset::extrudeAndValidateMainDataFromFile()
    {
        try
        {
            const auto json = nlohmann::json::parse(Utils::GetFileContent(_pathToSource));

            if (!json.contains(AssetData::type))
            {
                throw std::runtime_error("Asset's source file doesn't contain a field: 'type'.");
            }
            if (!json.contains(AssetData::data))
            {
                throw std::runtime_error("Asset's source file doesn't contain a field: 'data'.");
            }

            _type = StringAtom::Intern(json[AssetData::type].get<StringAtom>());
            Assert(_type.isStatic());
            if (_type.isEmpty())
            {
                throw std::runtime_error("Asset's source file contains empty 'type' field.");
            }

            if (json.contains(AssetData::name))
            {
                _name = json[AssetData::name].get<StringAtom>();
            }

            if (_name.isEmpty())
            {
                _name = "Asset_" + _type;
            }
            _name.shrinkToFit();

            auto& gcf = GetGlobalComponentFactory();
            auto& aif = AssetImpl::GetFactory();

            if (const auto id = gcf.getTypeIdByTypeName(_type))
            {
                _impl = aif.trySpawnImpl(id.value());
            }

            _status = Status::PreLoaded;
            traceLog("Successfully preloaded. New status is: 'PreLoaded'. Asset: {}"_f
                     << _logicPath);
        }
        catch (const std::exception& e)
        {
            criticalLog(
                "Can't parse asset's file: {}. Reason: {}. New status is: 'PreLoadingError'"_f
                << _pathToSource << e.what());
            _status = Status::PreLoadingError;
            _pathToSource.clear();
        }
        catch (...)
        {
            criticalLog(
                "Can't parse asset's file: {}. Due to internal error. New status is: 'PreLoadingError'"_f
                << _pathToSource);
            _status = Status::PreLoadingError;
            _pathToSource.clear();
        }
    }

    void ECSAsset::localClear()
    {
        unload();
    }

} // namespace Core