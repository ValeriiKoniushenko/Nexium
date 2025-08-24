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

#pragma once

#include "Misc/JsonCacheable.h"
#include "StaticMesh.h"

namespace Core
{
    class StaticMeshBundle : public BaseComponent, public JsonCacheable, public Transformable
    {
        ECS_REGISTER_NEW_COMPONENT(StaticMeshBundle, BaseComponent);

    public:
        using MeshesT = std::vector<StaticMesh*>;

    public:
        void draw();
        void importFrom(const aiNode* node, const aiScene* scene,
                        const std::filesystem::path& modelPath = "");
        void setShader(ShaderProgram* sp, bool ignoreVertexAttribSetup = false);
        void setOutlineShader(ShaderProgram* sp, bool ignoreVertexAttribSetup = false);

        void clear() override;

        void clearMeshes();

        [[nodiscard]] nlohmann::json toJson() const override;
        void fromJson(const nlohmann::json& json, bool isIgnoreChildren) override;

        [[nodiscard]] std::size_t getRenderTargetsCount() const noexcept { return _meshes.size(); }
        [[nodiscard]] MeshesT& getRenderTargets() noexcept { return _meshes; }
        [[nodiscard]] const MeshesT& getRenderTargets() const noexcept { return _meshes; }

    protected:
        [[nodiscard]] StringAtom getCacheHash() const override;
        [[nodiscard]] nlohmann::json toCacheData() const override;
        void fromCacheData(const nlohmann::json& data) override;

    protected:
        MeshesT _meshes;
    };

} // namespace Core