// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ECS/Transformable.h"
#include "Foundation/Interfaces/IOutliner.h"
#include "Misc/ITagHolder.h"
#include "NxFundamental/ECS/BaseComponent.h"
#include "RawBackend/IDrawable.h"

namespace Core
{

    struct SceneState
    {
        std::string name;
        Transformable trans;
        StringAtom assetType;
        StringAtom referenceAsset;
        nlohmann::json typeSpecificData;
    };

    void to_json(nlohmann::json& j, const SceneState& v);
    void from_json(const nlohmann::json& j, SceneState& v);

    CLASS();
    class SceneObject :
        public BaseComponent,
        public IOutliner,
        public Transformable,
        public IDrawable,
        public Foundation::IDataIO
    {
        ECS_DECL(SceneObject, NX::BaseComponent, Core::Transformable, Core::IOutliner,
                 Core::IDrawable);

    public:
        ~SceneObject() override = default;
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

        [[nodiscard]] virtual StringAtom shortStringify() const;

        void recalculateMatrices(const glm::mat4& mat = glm::mat4(1.f)) override;

        void onPreDeserialize(AbstractComponent* obj) override;
        void onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs) override;
        void onPreSerialize(const AbstractComponent* obj) const override;
        void onPostSerialize(const AbstractComponent* obj,
                             const RLogsCollector& logs) const override;

        [[nodiscard]] bool hasReferencedAsset() const noexcept;
        [[nodiscard]] StringAtom getReferencedAsset() const;
        void _setReferencedAsset(const StringAtom& logicPath);

        [[nodiscard]] virtual nlohmann::json getTypeSpecificSceneDataAsJson() const;
        virtual void applyTypeSpecificSceneData(const nlohmann::json& data);

        virtual void onAddedToScene() {}

        void draw(BaseCamera& camera) final;

        [[nodiscard]] virtual glm::vec3 getGlobalPosition() const;
        [[nodiscard]] virtual glm::vec3 getGlobalRotation() const;

        [[nodiscard]] StringAtom getCacheHash() const override { return "IDrawable"_atom; }

    protected:
        virtual void onDraw(BaseCamera& camera) {}

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
