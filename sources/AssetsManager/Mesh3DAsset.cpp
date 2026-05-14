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

#include "Mesh3DAsset.h"

#include "GameplaySystem/Framework/GameInstance.h"
#include "Misc/Configs.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace Assimp
{

} // namespace Assimp

namespace Core
{
    void Mesh3DAsset::onLoadRequest()
    {
        auto& sm = GetShaderManager();
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            (Config::Path::projectAbsPath / _pathToModel).string().c_str(), _assimpPostProcess);

        if (Verify(scene) && Verify(scene->mRootNode))
        {
            _data.importFrom(scene->mRootNode, scene, Config::Path::projectAbsPath / _pathToModel,
                             _onLoadScale);
            if (_mainShader && !_mainShader.isEmpty())
            {
                _data.setShader(sm.getShaderProgram(_mainShader));
            }
            if (_outlineShader && !_outlineShader.isEmpty())
            {
                _data.setOutlineShader(sm.getShaderProgram(_outlineShader));
            }

            applyAssetSettingsToObject();
        }
    }

    void Mesh3DAsset::onUnloadRequest()
    {
        _data.clear();
    }

    bool Mesh3DAsset::onHotReload()
    {
        applyAssetSettingsToObject();
        return true;
    }

    const std::filesystem::path& Mesh3DAsset::getPathToMode() const noexcept
    {
        return _pathToModel;
    }

    const StringAtom& Mesh3DAsset::getMainShader() const noexcept
    {
        return _mainShader;
    }

    const StringAtom& Mesh3DAsset::getOutlineShader() const noexcept
    {
        return _outlineShader;
    }

    float Mesh3DAsset::getOnLoadScale() const noexcept
    {
        return _onLoadScale;
    }

    int Mesh3DAsset::getAssimpPostProcessFlags() const noexcept
    {
        return _assimpPostProcess;
    }

    void Mesh3DAsset::setPathToModel(const std::filesystem::path& value)
    {
        _pathToModel = value;
    }

    void Mesh3DAsset::setMainShader(const StringAtom& value)
    {
        _mainShader = value;
    }

    void Mesh3DAsset::setOutlineShader(const StringAtom& value)
    {
        _outlineShader = value;
    }

    void Mesh3DAsset::setOnLoadScale(float value)
    {
        _onLoadScale = value;
    }

    void Mesh3DAsset::setAssimpPostProcessFlags(int value)
    {
        _assimpPostProcess = value;
    }

    /*
    void Mesh3DAsset::ioFieldsUpdate(DataStream& out)
    {
        BaseActorAsset::ioFieldsUpdate(out);

        auto stream = out.dedicatedNesting("Mesh3DAsset");

        stream.field("path", _pathToModel);
        stream.field("mainShader", _mainShader);
        if (stream.getMode() == DataStream::Mode::Input)
        {
            _mainShader = StringAtom::Intern(_mainShader);
        }
        stream.field("outlineShader", _outlineShader);
        if (stream.getMode() == DataStream::Mode::Input)
        {
            _outlineShader = StringAtom::Intern(_outlineShader);
        }
        stream.field("onLoadScale", _onLoadScale);
        stream.field(
            "assimpPostProcess", _assimpPostProcess,
            [this](int& out, nlohmann::json& s) { out = readAssimpPostProcessFromCache(s); },
            [this](int assimpFlags) { return assimpPostProcessToCache(assimpFlags); });
    }*/

    int Mesh3DAsset::readAssimpPostProcessFromCache(nlohmann::json& s)
    {
        int out = 0;
        for (const auto& el : s)
        {
            const auto asString = el.get<StringAtom>();
            // if (auto value = Assimp::aiPostProcessStepsFromString(asString))
            // {
            //     out |= *value;
            // }
            // else
            // {
            //     errorLog("Invalid value '{}' for property 'assimpPostProcess'"_f << asString);
            //     Assert(false);
            // }
        }

        return out;
    }

    nlohmann::json Mesh3DAsset::assimpPostProcessToCache(int assimpFlags)
    {
        auto out = nlohmann::json::array();

        for (std::size_t i = 0; i < sizeof(int) * 8; ++i)
        {
            const auto flag = static_cast<aiPostProcessSteps>((1 << i) & assimpFlags);
            if (flag != 0)
            {
            }
        }

        return out;
    }

} // namespace Core