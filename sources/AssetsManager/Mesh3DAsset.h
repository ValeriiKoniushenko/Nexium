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

#pragma once

#include "BaseActorAsset.h"
#include "BaseAsset.h"
#include "Graphics/GraphicsComponents.h"
#include "Graphics/Primitives/StaticMeshBundle.h"
#include "Graphics/Texture.h"
#include "assimp/postprocess.h"

namespace Assimp
{
    [[nodiscard]] Core::StringAtom aiPostProcessStepsToString(aiPostProcessSteps);

    [[nodiscard]] std::optional<aiPostProcessSteps> aiPostProcessStepsFromString(
        const Core::StringAtom& val);

    extern const std::vector<aiPostProcessSteps> aiPostProcessStepsAsVector;
} // namespace Assimp

namespace Core
{
    class Mesh3DAsset : public BaseActorAsset<StaticMeshBundle>
    {
    public:
        inline static const char* fileExtension = ".nxmesh3d";

    public:
        explicit Mesh3DAsset(const StringAtom& logicPath)
            : BaseActorAsset(logicPath)
        {
        }

        ~Mesh3DAsset() override = default;

        void onLoadRequest() override;

        void onUnloadRequest() override;

        [[nodiscard]] const char* getPrefix() const override { return "Mesh3D"; }

        [[nodiscard]] const std::filesystem::path& getPathToMode() const noexcept;
        [[nodiscard]] const StringAtom& getMainShader() const noexcept;
        [[nodiscard]] const StringAtom& getOutlineShader() const noexcept;
        [[nodiscard]] int getAssimpPostProcessFlags() const noexcept;
        [[nodiscard]] float getOnLoadScale() const noexcept;

        void setPathToModel(const std::filesystem::path& value);
        void setMainShader(const StringAtom& value);
        void setOutlineShader(const StringAtom& value);
        void setAssimpPostProcessFlags(int value);
        void setOnLoadScale(float value);

    protected:
        void ioFieldsUpdate(DataStream& stream) override;

    private:
        [[nodiscard]] int readAssimpPostProcessFromCache(nlohmann::json& cache);
        [[nodiscard]] nlohmann::json assimpPostProcessToCache(int assimpFlags);

    protected: // properties
        std::filesystem::path _pathToModel;
        StringAtom _mainShader;
        StringAtom _outlineShader;
        int _assimpPostProcess = 0;
        float _onLoadScale = 1.f;
    };

    using NXMesh3D = AssetRef<Mesh3DAsset>;
} // namespace Core