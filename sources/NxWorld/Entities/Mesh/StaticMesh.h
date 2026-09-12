// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Foundation/Interfaces/IOutliner.h"
#include "NxFundamental/ECS/BaseComponent.h"
#include "NxFundamental/Transformable.h"
#include "NxSubsystems/Graphics/GraphicsComponents.h"
#include "assimp/matrix4x4.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

#include <filesystem>

namespace NX
{
    class StaticMeshBundle;
    class BaseCamera;

    CLASS();
    class StaticMesh :
        public InterleavedGraphicsData,
        public Transformable,
        public BaseComponent,
        public Foundation::IOutliner
    {
        ECS_DECL(StaticMesh, NX::BaseComponent, NX::Transformable, NX::InterleavedGraphicsData);

    public:
        constexpr static Core::Color4 outlineColor = Core::Color4{ 252, 186, 3, 255 };
        constexpr static float outlineSize = 40.f;

    public:
        void importFrom(const aiMesh* rawMesh, const aiScene* scene,
                        const std::filesystem::path& modelPath = "", float scale = 1.f);

        [[nodiscard]] Core::FSize3 getSize() const noexcept { return _size; }
        [[nodiscard]] glm::vec3 getCenter() const noexcept { return _center; }

        [[nodiscard]] ShaderProgram* getOutlineShader() noexcept { return _outlineShader; }

        void setOutlineShader(ShaderProgram* sp, bool ignoreVertexAttribSetup = false);

        [[nodiscard]] StaticMeshBundle* tryToGetRootBundle();

        [[nodiscard]] const StaticMeshBundle* tryToGetRootBundle() const;

        /// will draw with the default shader & logic. Single draw object!
        void draw(BaseCamera& camera);

        /// only draw call with graphics modifiers - nothing more.
        /// Before using of this function you must manually prepare the shader &
        /// 'use' it.
        void pureDraw(const std::function<void(StaticMesh*)>& onUniformSet);

        void generate() override;
        void clear() override;

        [[nodiscard]] Core::Color3 toUniqueColor() const noexcept
        {
            const auto id = const_cast<StaticMesh*>(this)->getVboId()
                            ^ reinterpret_cast<std::uintptr_t>(this);

            Color3 colorId;
            colorId.r = static_cast<uint8_t>((id & 0x0000FF) >> 0);
            colorId.g = static_cast<uint8_t>((id & 0x00FF00) >> 8);
            colorId.b = static_cast<uint8_t>((id & 0xFF0000) >> 16);

            return colorId;
        }

        [[nodiscard]] bool isMatchUniqueColor(Core::Color3 color) const noexcept
        {
            return toUniqueColor() == color;
        }

        void recalculateMatrices(const glm::mat4& mat = glm::mat4(1.f)) override;

        [[nodiscard]] Core::StringAtom getCacheHash() const override;

    protected:
        void onDirtyMatrix() override;

        void onOutlineStatusChange(bool) override {}

        void calculateSizeBaseOnMesh(const aiMesh* rawMesh, const aiMatrix4x4& transform);

        void drawOutline(BaseCamera& camera);

    protected:
        FIELD();
        Core::FSize3 _size;

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

        [[nodiscard]] static StaticMesh CreateBase(const Core::StringAtom& name = ""_atom);

        [[nodiscard]] static StaticMesh CreateBiSide(const Core::StringAtom& name = ""_atom);

        [[nodiscard]] static StaticMesh CreateBiBlendSide(const Core::StringAtom& name = ""_atom);
    };
} // namespace NX

#include "StaticMesh.generated.h" // added by the code generator. Better don't move it.
