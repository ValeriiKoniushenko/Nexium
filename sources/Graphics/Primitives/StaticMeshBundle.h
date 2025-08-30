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

#include "GameplaySystem/Framework/Actor.h"
#include "Misc/JsonCacheable.h"
#include "StaticMesh.h"

namespace Core
{
    class StaticMeshBundle : public Actor, public IOutliner
    {
        ECS_REGISTER_NEW_COMPONENT(StaticMeshBundle, Actor);

    public:
        /**
         * @brief will draw with default shader & logic. Single draw bundle!
         */
        void draw() override;

        /**
         * @brief only draw call with graphics modifiers - nothing more.
         * Before using of this function you must manually prepare the shader &
         * 'use' it.
         */
        void pureDraw() override;

        void importFrom(const aiNode* node, const aiScene* scene,
                        const std::filesystem::path& modelPath = "");
        void setShader(ShaderProgram* sp, bool ignoreVertexAttribSetup = false);
        void setOutlineShader(ShaderProgram* sp, bool ignoreVertexAttribSetup = false);

        void clear() override;

        void clearMeshes();

        [[nodiscard]] nlohmann::json toJson() const override;
        void fromJson(const nlohmann::json& json, bool isIgnoreChildren) override;

        [[nodiscard]] std::size_t getRenderTargetsCount() const noexcept { return _meshes.size(); }
        [[nodiscard]] std::vector<StaticMesh*>& getRenderTargets() noexcept { return _meshes; }
        [[nodiscard]] const std::vector<StaticMesh*>& getRenderTargets() const noexcept
        {
            return _meshes;
        }

        [[nodiscard]] uint32_t getID() const noexcept { return _id; }

        void recalculateMatrices(const glm::mat4& mat = glm::mat4(1.f)) override;

    protected:
        void onOutlineStatusChange(bool newStatus) override;
        [[nodiscard]] StringAtom getCacheHash() const override;
        [[nodiscard]] nlohmann::json toCacheData() const override;
        void fromCacheData(const nlohmann::json& data) override;

        void onAddChild(BaseComponent* newChild) override;
        void onRemoveChild(BaseComponent* child) override;

    protected:
        std::vector<StaticMesh*> _meshes;
        std::vector<StaticMeshBundle*> _bundles;

        inline static uint32_t _idGenerator = 0;
        uint32_t _id = ++_idGenerator;
    };

} // namespace Core