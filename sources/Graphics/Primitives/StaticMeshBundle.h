/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "GameplaySystem/Entities/Actor.h"
#include "StaticMesh.h"

struct aiScene;
struct aiNode;

namespace Core
{

    CLASS();
    class StaticMeshBundle : public Actor
    {
        ECS_DECL(StaticMeshBundle, Core::Actor);

    public:
        ~StaticMeshBundle() override;
        StaticMeshBundle(const StaticMeshBundle& other);
        StaticMeshBundle(StaticMeshBundle&& other) = default;
        StaticMeshBundle& operator=(const StaticMeshBundle& other);
        StaticMeshBundle& operator=(StaticMeshBundle&& other) = default;

        /// only draw call with graphics modifiers - nothing more.
        /// Before using of this function you must manually prepare the shader &
        /// 'use' it.
        void pureDraw(const std::function<void(StaticMesh*)>& onUniformSet,
                      const std::function<bool(const Actor*)>& conditional) override;

        void importFrom(const aiNode* node, const aiScene* scene,
                        const std::filesystem::path& modelPath = "", float scale = 1.f);

        void setShader(ShaderProgram* sp, bool ignoreVertexAttribSetup = false);
        void setOutlineShader(ShaderProgram* sp, bool ignoreVertexAttribSetup = false);

        void onTick(float delta) override;

        void clear() override;

        void clearMeshes();

        [[nodiscard]] std::size_t getRenderMeshesCount() const noexcept { return _meshes.size(); }
        [[nodiscard]] std::vector<StaticMesh*>& getRenderMeshes() noexcept { return _meshes; }

        [[nodiscard]] const std::vector<StaticMesh*>& getRenderMeshes() const noexcept
        {
            return _meshes;
        }

        [[nodiscard]] std::size_t getRenderableTriangles() const;

        [[nodiscard]] std::size_t getRenderBundlesCount() const noexcept { return _bundles.size(); }

        [[nodiscard]] std::vector<StaticMeshBundle*>& getRenderBundles() noexcept
        {
            return _bundles;
        }

        [[nodiscard]] const std::vector<StaticMeshBundle*>& getRenderBundles() const noexcept
        {
            return _bundles;
        }

        /// It controls will be object selected by ObjectSelectorManager or no
        /// @return true - will be ignored; false - otherwise
        [[nodiscard]] bool isIgnoreSelect() const noexcept { return _ignoreSelect; }

        /// It controls will be object selected by ObjectSelectorManager or no
        /// @param value true - will be ignored; false - otherwise
        void setIsIgnoreSelect(bool value) noexcept { _ignoreSelect = value; }

        [[nodiscard]] uint32_t getID() const noexcept { return _id; }

        void recalculateMatrices(const glm::mat4& mat = glm::mat4(1.f)) override;

        /// This function will be called after a mouse click on this object.
        virtual void onMousePicked(StaticMesh* clickedPart) {}

        friend void swap(StaticMeshBundle& a, StaticMeshBundle& b) noexcept
        {
            using std::swap;
            swap(static_cast<Actor&>(a), static_cast<Actor&>(b));
            swap(a._meshes, b._meshes);
            swap(a._bundles, b._bundles);
            swap(a._id, b._id);
            swap(a._ignoreSelect, b._ignoreSelect);
        }

    protected:
        void onDraw(BaseCamera&) override;
        void onOutlineStatusChange(bool newStatus) override;
        void onAddChild(BaseComponent* newChild) override;
        void onRemoveChild(BaseComponent* child) override;
        void invalidateFastAccessContainers();

    protected:
        static uint32_t _idGenerator;

        std::vector<StaticMesh*> _meshes;
        std::vector<StaticMeshBundle*> _bundles;
        uint32_t _id = ++_idGenerator;

        FIELD();
        bool _ignoreSelect = false;

    private:
        void privateClear();
    };
} // namespace Core

#include "StaticMeshBundle.generated.h" // added by the code generator. Better don't move it.
