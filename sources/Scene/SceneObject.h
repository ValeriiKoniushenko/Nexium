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

#include "AssetsManager/ITagHolder.h"
#include "ECS/BaseComponent.h"
#include "ECS/Transformable.h"
#include "Graphics/IDrawable.h"
#include "Graphics/IOutliner.h"

namespace Core
{

    struct SceneState
    {
        std::string name;
        Transformable trans;
        StringAtom assetType;
        StringAtom referenceAsset;
    };

    void to_json(nlohmann::json& j, const SceneState& v);
    void from_json(const nlohmann::json& j, SceneState& v);

    CLASS();
    class SceneObject :
        public BaseComponent,
        public IOutliner,
        public Transformable,
        public IDrawable,
        public ITagHolder
    {
        ECS_COMPONENT_DECL(SceneObject, BaseComponent);
        R_FRIEND_DECL(Core::SceneObject, Core::BaseComponent, Core::Transformable, Core::IOutliner,
                      Core::IDrawable);

    public:
        ~SceneObject() override
        {
            ///
            int i = 123;
        }
        SceneObject(const SceneObject&) = default;
        SceneObject(SceneObject&&) = default;
        SceneObject& operator=(const SceneObject&) = default;
        SceneObject& operator=(SceneObject&&) = default;

        [[nodiscard]] spdlog::logger* getLogger() const override;

        [[nodiscard]] SceneState getSceneState() const;

        friend void swap(SceneObject& a, SceneObject& b) noexcept
        {
            using std::swap;
            swap(static_cast<BaseComponent&>(a), static_cast<BaseComponent&>(b));
            swap(static_cast<IOutliner&>(a), static_cast<IOutliner&>(b));
            swap(static_cast<Transformable&>(a), static_cast<Transformable&>(b));
            swap(static_cast<IDrawable&>(a), static_cast<IDrawable&>(b));
        }

        [[nodiscard]] Tag getTags() const override;

        [[nodiscard]] virtual StringAtom stringify() const;

        void recalculateMatrices(const glm::mat4& mat = glm::mat4(1.f)) override;

        void onPreDeserialize(AbstractComponent* obj) override;
        void onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs) override;
        void onPreSerialize(const AbstractComponent* obj) const override;
        void onPostSerialize(const AbstractComponent* obj,
                             const RLogsCollector& logs) const override;

        [[nodiscard]] bool hasReferencedAsset() const noexcept;
        [[nodiscard]] StringAtom getReferencedAsset() const;
        void _setReferencedAsset(const StringAtom& logicPath);

    protected:
        void makeTransformableTreeDirty();
        void onInitialize() override;
        void onPreInitialize() override;
        void onOutlineStatusChange(bool newStatus) override;

        void onDirtyMatrix() override;

    private:
        StringAtom _referencedAsset;
    };

} // namespace Core

#include "SceneObject.generated.h" // added by the code generator. Better don't move it.
