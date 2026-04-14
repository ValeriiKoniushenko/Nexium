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

#include "ECS/BaseComponent.h"
#include "ECS/Transformable.h"
#include "Graphics/GraphicsComponents.h"
#include "Graphics/IOutliner.h"
#include "assimp/matrix4x4.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

#include <filesystem>

namespace Core
{
    class StaticMeshBundle;

    CLASS();
    class StaticMesh :
        public GraphicsComponentData,
        public Transformable,
        public BaseComponent,
        public IOutliner
    {
        ECS_COMPONENT_DECL(StaticMesh, BaseComponent);
        ECS_R_FRIEND_DECL(StaticMesh, Core::BaseComponent, Core::Transformable,
                          Core::GraphicsComponentData);

    public:
        constexpr static Color4 outlineColor = Color4{ 252, 186, 3, 255 };
        constexpr static float outlineSize = 40.f;

    public:
        void importFrom(const aiMesh* rawMesh, const aiScene* scene,
                        const std::filesystem::path& modelPath = "", float scale = 1.f);

        [[nodiscard]] FSize3 getSize() const noexcept { return _size; }
        [[nodiscard]] glm::vec3 getCenter() const noexcept { return _center; }

        [[nodiscard]] ShaderProgram* getOutlineShader() noexcept { return _outlineShader; }

        void setOutlineShader(ShaderProgram* sp, bool ignoreVertexAttribSetup = false);

        [[nodiscard]] StaticMeshBundle* tryToGetRootBundle();

        [[nodiscard]] const StaticMeshBundle* tryToGetRootBundle() const;

        /**
         * will draw with the default shader & logic. Single draw object!
         */
        void draw();

        /**
         * only draw call with graphics modifiers - nothing more.
         * Before using of this function you must manually prepare the shader &
         * 'use' it.
         */
        void pureDraw(const std::function<void(StaticMesh*)>& onUniformSet);

        void generate() override;
        void clear() override;

        [[nodiscard]] Color3 toUniqueColor() const noexcept
        {
            const auto id = _vbo ^ reinterpret_cast<std::uintptr_t>(this);

            Color3 colorId;
            colorId.r = static_cast<uint8_t>((id & 0x0000FF) >> 0);
            colorId.g = static_cast<uint8_t>((id & 0x00FF00) >> 8);
            colorId.b = static_cast<uint8_t>((id & 0xFF0000) >> 16);

            return colorId;
        }

        [[nodiscard]] bool isMatchUniqueColor(Color3 color) const noexcept
        {
            return toUniqueColor() == color;
        }

        void recalculateMatrices(const glm::mat4& mat = glm::mat4(1.f)) override;

    protected:
        [[nodiscard]] StringAtom getCacheHash() const override;

        void onDirtyMatrix() override;

        void onOutlineStatusChange(bool) override {}

        void calculateSizeBaseOnMesh(const aiMesh* rawMesh, const aiMatrix4x4& transform);

        void drawOutline();

    protected:
        FIELD();
        FSize3 _size;

        FIELD();
        glm::vec3 _center = glm::vec3(0);

        ShaderProgram* _outlineShader = nullptr;
        GLuint _uboLight = 0;

        friend class StaticMeshFactory;
    };

    class StaticMeshFactory
    {
    public:
        StaticMeshFactory() = delete;

        [[nodiscard]] static StaticMesh CreateBase(const StringAtom& name = ""_atom);

        [[nodiscard]] static StaticMesh CreateBiSide(const StringAtom& name = ""_atom);

        [[nodiscard]] static StaticMesh CreateBiBlendSide(const StringAtom& name = ""_atom);
    };
} // namespace Core

#include "StaticMesh.generated.h" // added by the code generator. Better don't move it.
