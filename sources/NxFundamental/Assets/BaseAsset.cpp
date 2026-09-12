// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "BaseAsset.h"

#include "../PrivateModuleInfo.h"
#include "Foundation/Configs.h"
#include "Utils/Functions.h"

#include <fstream>

using namespace Core;

namespace NX
{

    void BaseAsset::loadRequest()
    {
        traceLog("Loading of: {}"_f << _assetPath.string());
        onLoadRequest();
    }

    void BaseAsset::unloadRequest()
    {
        traceLog("Unloading of: {}"_f << _assetPath.string());
        onUnloadRequest();
    }

    void BaseAsset::makeHotReload()
    {
        traceLog("Hot reload for: {}"_f << _assetPath.string());
        if (onHotReload())
        {
            infoLog("Successfull hot reload for: {}"_f << _assetPath.string());
        }
        else
        {
            warnLog("This asset doesn't support hot-reload: {}"_f << _assetPath.string());
        }
    }

    void BaseAsset::attachAndReadFromFile(const std::filesystem::path& path)
    {
        attachSourceFile(path);

        try
        {
            RResourceStream<RJsonResourceStream> stream;
            stream.getData()
                = nlohmann::json::parse(Utils::GetTextFileContentAs<std::string>(_assetPath));

            auto assetData = stream.getData().find("AssetData");
            if (assetData == stream.getData().end())
            {
                throw std::runtime_error(
                    "Asset's source file doesn't contain a field: 'AssetData'.");
            }

            RResourceStream<RJsonResourceStream> data(*assetData);
            deserialize(data);
            if (!data.logs().empty())
            {
                warnLog("{} field(s) couldn't be deserialized. The asset: {} "_f
                        << data.logs().size() << getLogicPath());
                for (auto&& [field, code] : data.logs())
                {
                    warnLog("Field '{}' - {} "_f << field << RStatusToString(code));
                }
            }
        }
        catch (const std::exception& e)
        {
            criticalLog("Can't read file: {}. Reason: {}"_f << _assetPath << e.what());
        }
        catch (...)
        {
            criticalLog("Can't read file: {}. Reason is undefined."_f << _assetPath);
        }
    }

    void BaseAsset::attachSourceFile(const std::filesystem::path& path)
    {
        _assetPath = path;

        if (_assetPath.empty())
        {
            criticalLog("Invalid path to asset: {}"_f << _assetPath);
        }
        else if (!std::filesystem::exists(_assetPath))
        {
            criticalLog("Asset doesn't exist: {}"_f << _assetPath);
        }
    }

    void BaseAsset::writeToFile()
    {
        if (_assetPath.empty())
        {
            criticalLog("Invalid path to asset: {}"_f << _assetPath);
            return;
        }

        DataStream stream;
        stream.setMode(DataStream::Mode::Output);
        stream.getRaw()
            = nlohmann::json::parse(Utils::GetTextFileContentAs<std::string>(_assetPath));
        ioFieldsUpdate(stream);

        const auto data = stream.getRaw().dump(4);
        std::ofstream out(_assetPath);
        if (!out.is_open())
        {
            criticalLog("Can't open file for write: {}"_f << _assetPath);
            return;
        }
        out.write(data.c_str(), static_cast<std::streamsize>(data.length()));
    }

    spdlog::logger* BaseAsset::getLogger() const
    {
        return NxFundamental::getLogger();
    }

    nlohmann::json BaseAsset::serialize() const
    {
        return R<BaseAsset>::Serialize(*this).getData();
    }

    void BaseAsset::deserialize(RResourceStream<RJsonResourceStream>& data)
    {
        R<BaseAsset>::Deserialize(data, *this);
    }

} // namespace NX