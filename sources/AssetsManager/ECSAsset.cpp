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
#include "Utils/Functions.h"
#include "nlohmann/json.hpp"

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
        _meta.pathToSource = src;

        if (_meta.pathToSource.empty())
        {
            criticalLog("Empty path was passed to asset's sources");
            return;
        }

        if (!std::filesystem::exists(_meta.pathToSource))
        {
            criticalLog("Asset's source file doesn't exist: {}"_f << _meta.pathToSource);
            _meta.pathToSource.clear();
            return;
        }

        extrudeAndValidateMainDataFromFile();
    }

    const std::filesystem::path& ECSAsset::getSourceFile() const noexcept
    {
        return _meta.pathToSource;
    }

    nlohmann::json ECSAsset::getAssetData() const
    {
        try
        {
            const auto json = nlohmann::json::parse(Utils::GetFileContent(_meta.pathToSource));

            if (!json.contains(StreamData::assetData))
            {
                return {};
            }

            return json[StreamData::assetData];
        }
        catch (const std::exception& e)
        {
            criticalLog("Can't provide asset's data: {}. Reason: {}"_f << _meta.pathToSource
                                                                       << e.what());
        }
        catch (...)
        {
            criticalLog("Can't provide asset's data: {}. Due to internal error."_f
                        << _meta.pathToSource);
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

        if (_status != Status::Loaded && _status != Status::PreLoaded)
        {
            warnLog("Can't sync asset with memory. Asset is not loaded properly.");
            return;
        }

        nlohmann::json json;
        json[StreamData::type] = _meta.type;
        json[StreamData::name] = _meta.name;
        json[StreamData::data] = nlohmann::json::object();

        json[StreamData::assetData] = assetData;

        if (_status == Status::Loaded)
        {
            // DataStream stream;
            // stream.setMode(DataStream::Mode::Output);
            // _data->ioFieldsUpdate(stream);
            //
            // json[StreamData::name] = _data->getComponentName();
            // json[StreamData::data] = std::move(stream.getRaw());
        }

        std::fstream out(_meta.pathToSource, std::ios::out);
        if (out.is_open())
        {
            out << json.dump(4);
            traceLog("Asset: {} was updated successfully"_f << _meta.logicPath);
        }
        else
        {
            criticalLog("Can't open file for write: {} - to update Asset:: {}"_f
                        << _meta.pathToSource << _meta.logicPath);
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

        _impl->processAction(action, *this);
    }

    bool ECSAsset::operator==(const ECSAsset& other) const
    {
        return other._meta.logicPath == _meta.logicPath;
    }

    bool ECSAsset::operator==(const IntrusivePtr<ECSAsset>& other) const
    {
        if (Verify(other)) [[likely]]
        {
            return operator==(*other);
        }
        return false;
    }

    void ECSAsset::PackObjectToAsset(ECSAsset& out, const BaseComponent* data)
    {
        if (!Verify(data)) [[unlikely]]
        {
            globalLog.errorLog("Was passed nullptr to ECSAsset::PackObjectToAsset");
            return;
        }

        out._data = data->clone();
        out._meta.type = data->getComponentType();
        out._meta.name = data->getComponentName();
    }

    nlohmann::json ECSAsset::toJson() const
    {
        nlohmann::json j;

        j[StreamData::type] = _meta.type;
        j[StreamData::name] = _meta.name;
        j[StreamData::assetData] = nlohmann::json::object();

        j[StreamData::data] = _data->serialize();

        return j;
    }

    void ECSAsset::load()
    {
        if (_status != Status::PreLoaded)
        {
            warnLog("Can't load asset: '{}'. Status is not 'PreLoaded'."_f << _meta.logicPath);
            return;
        }

        try
        {
            traceLog("Loading of asset: {}"_f << _meta.logicPath);

            _data.reset();
            _data = GetGlobalComponentFactory().create(_meta.type);
            if (!_data)
            {
                throw std::runtime_error(
                    "Can't parse & recreate RTTI type. Maybe, the ECS type was not registered, or "
                    "type is template(it can cause problems).");
            }

            // Fetching main asset's data
            const auto json = nlohmann::json::parse(
                Utils::GetTextFileContentAs<std::string>(_meta.pathToSource));

            RResourceStream<RJsonResourceStream> data(json[StreamData::data]);
            _data->deserialize(data);

            // [opt] making loading of essential data (texture loading, 3D model loading, etc)
            if (_impl)
            {
                _impl->load(*this, _data.get(), json[StreamData::assetData]);
            }

            if (!data.logs().empty())
            {
                warnLog("{} field(s) couldn't be deserialized. The asset: {} "_f
                        << data.logs().size() << _meta.logicPath);
                for (auto&& [field, code] : data.logs())
                {
                    warnLog("Field '{}' - {} "_f << field << RStatusToString(code));
                }
            }

            _status = Status::Loaded;
            traceLog("Asset:: '{}' is: loaded! New status is: 'Loaded'"_f << _meta.logicPath);
        }
        catch (const std::exception& e)
        {
            criticalLog("Can't load Asset:: '{}'. The reason: {}. New status is: 'LoadingError'"_f
                        << _meta.logicPath << e.what());
            _status = Status::LoadingError;
        }
        catch (...)
        {
            criticalLog(
                "Can't load Asset:: '{}'. The reason is undefined. New status is: 'LoadingError'"_f
                << _meta.logicPath);
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
                 << _meta.logicPath);
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
            const auto json = nlohmann::json::parse(Utils::GetFileContent(_meta.pathToSource));

            if (!json.contains(StreamData::type))
            {
                throw std::runtime_error("Asset's source file doesn't contain a field: 'type'.");
            }
            if (!json.contains(StreamData::data))
            {
                throw std::runtime_error("Asset's source file doesn't contain a field: 'data'.");
            }

            _meta.type = StringAtom::Intern(json[StreamData::type].get<StringAtom>());
            if (_meta.type.isEmpty())
            {
                throw std::runtime_error("Asset's source file contains empty 'type' field.");
            }

            if (json.contains(StreamData::name))
            {
                _meta.name = json[StreamData::name].get<StringAtom>();
            }
            else
            {
                _meta.name = "Asset_" + _meta.pathToSource.stem().generic_string();
            }
            _meta.name.shrinkToFit();

            if (const auto id = GetGlobalComponentFactory().getTypeIdByTypeName(_meta.type))
            {
                _impl = AssetImpl::GetFactory().trySpawnImpl(id.value());
            }

            _status = Status::PreLoaded;
            traceLog("Successfully preloaded. New status is: 'PreLoaded'. Asset: {}"_f
                     << _meta.logicPath);
        }
        catch (const std::exception& e)
        {
            criticalLog(
                "Can't parse asset's file: {}. Reason: {}. New status is: 'PreLoadingError'"_f
                << _meta.pathToSource << e.what());
            _status = Status::PreLoadingError;
            _meta.pathToSource.clear();
        }
        catch (...)
        {
            criticalLog(
                "Can't parse asset's file: {}. Due to internal error. New status is: 'PreLoadingError'"_f
                << _meta.pathToSource);
            _status = Status::PreLoadingError;
            _meta.pathToSource.clear();
        }
    }

    void ECSAsset::localClear()
    {
        unload();
    }

    NXSceneAsset::NXSceneAsset(const NXSceneAsset& other)
        : IntrusiveRefCounter(other),
          _asset(other._asset),
          _data(other._data->clone())
    {
    }

    NXSceneAsset& NXSceneAsset::operator=(const NXSceneAsset& other)
    {
        if (&other == this) [[unlikely]]
        {
            return *this;
        }

        _asset = other._asset;
        _data = other._data->clone();

        return *this;
    }

    void NXSceneAsset::setAsset(ECSAsset& asset)
    {
        _data.reset();

        _asset = &asset;
        if (_asset && _asset->getData())
        {
            _data = _asset->getData()->clone();
        }
    }

    void NXSceneAsset::setAsset(NXAsset asset)
    {
        if (Verify(asset)) [[likely]]
        {
            setAsset(*asset);
        }
    }

    bool NXSceneAsset::setData(const BaseComponent* comp)
    {
        if (!validateInputSetData(comp))
        {
            return false;
        }

        _data = comp->clone();
        return true;
    }

    bool NXSceneAsset::validateInputSetData(const BaseComponent* comp)
    {
        if (!Verify(comp)) [[unlikely]]
        {
            return false;
        }

        if (_asset && _asset->getData())
        {
            if (_asset->getData()->getComponentType() != comp->getComponentType())
            {
                Assert(_asset->getData()->getComponentType().isStatic());
                Assert(comp->getComponentType().isStatic());

                globalLog.errorLog(
                    "Attempt to assign data of type '{}' to different type of the asset '{}'"_f
                    << comp->getComponentType() << _asset->getData()->getComponentType());

                return false;
            }
        }

        return true;
    }

} // namespace Core