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
#include "ModuleInfo.h"
#include "Utils/Functions.h"

#include <fstream>


#include "BaseAsset.generated.cpp.inl" // this line added by the code generator.

namespace Core
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
            DataStream stream;
            stream.setMode(DataStream::Mode::Input);
            stream.getRaw()
                = nlohmann::json::parse(Utils::GetTextFileContentAs<std::string>(_assetPath));
            ioFieldsUpdate(stream);
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
        return AssetsManager::getLogger();
    }

} // namespace Core