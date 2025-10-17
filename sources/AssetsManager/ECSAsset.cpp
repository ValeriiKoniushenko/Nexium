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

#include <Utils/Functions.h>
#include <nlohmann/json.hpp>

namespace Core
{

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

        try
        {
            extrudeAndValidateMainDataFromFile();
        }
        catch (std::exception e)
        {
            criticalLog("Can't parse asset's file: {}. Reason: {}"_f << _pathToSource << e.what());
            _status = Status::LoadingError;
            _pathToSource.clear();
        }
        catch (...)
        {
            criticalLog("Can't parse asset's file: {}. Due to internal error."_f << _pathToSource);
            _status = Status::LoadingError;
            _pathToSource.clear();
        }
    }

    bool ECSAsset::isLoaded() const noexcept
    {
        return _status.cast() == Status::Loaded;
    }

    bool ECSAsset::hasLoadingError() const noexcept
    {
        return _status.cast() == Status::LoadingError;
    }

    void ECSAsset::extrudeAndValidateMainDataFromFile()
    {
        const auto json = nlohmann::json::parse(Utils::GetFileContent(_pathToSource));

        if (!json.contains("type"))
        {
            throw std::runtime_error("Asset's source file doesn't contain a field: 'type'.");
        }
        if (!json.contains("data"))
        {
            throw std::runtime_error("Asset's source file doesn't contain a field: 'data'.");
        }

        _type = StringAtom::Intern(json["type"].get<StringAtom>());
        DEBUG_ASSERT(_type.isStatic());
        if (_type.isEmpty())
        {
            throw std::runtime_error("Asset's source file contains empty 'type' field.");
        }

        if (json.contains("name"))
        {
            _name = json["name"].get<StringAtom>();
        }

        if (_name.isEmpty())
        {
            _name = "Asset_" + _type;
        }
        _name.shrinkToFit();
    }

} // namespace Core