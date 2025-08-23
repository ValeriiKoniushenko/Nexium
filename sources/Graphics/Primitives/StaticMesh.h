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

#include "../GraphicsComponents.h"
#include "GameplaySystem/ECS/BaseComponent.h"
#include "GameplaySystem/ECS/Transformable.h"
#include "assimp/matrix4x4.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

#include <filesystem>

namespace Core
{
    class StaticMesh : public GraphicsComponentData, public Transformable, public BaseComponent
    {
        ECS_REGISTER_NEW_COMPONENT(StaticMesh, BaseComponent);

    public:
        constexpr static Color4 outlineColor = Color4{252, 186, 3, 255};
        constexpr static float outlineSize = 40.f;

    public:
        void importFrom(const aiMesh* mesh, const aiScene* scene,
                        const std::filesystem::path& modelPath = "");

        [[nodiscard]] FSize3 getSize() const noexcept { return _size; }
        [[nodiscard]] glm::vec3 getCenter() const noexcept { return _center; }

        [[nodiscard]] ShaderProgram* getOutlineShader() noexcept { return _outlineShader; }
        void setOutlineShader(ShaderProgram* sp, bool ignoreVertexAttribSetup = false);

        void draw();

        [[nodiscard]] nlohmann::json toJson() const override;
        void fromJson(const nlohmann::json& json, bool isIgnoreChildren) override;

    protected:
        void applyUniforms() override;
        void calculateSizeBaseOnMesh(const aiMesh* rawMesh, const aiMatrix4x4& transform);
        void drawOutline();

    protected:
        FSize3 _size;
        glm::vec3 _center = glm::vec3(0);
        ShaderProgram* _outlineShader = nullptr;
        bool _isDrawOutline = true;

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