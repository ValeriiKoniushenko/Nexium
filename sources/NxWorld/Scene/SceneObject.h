// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Foundation/Interfaces/DataStream.h"
#include "Foundation/Interfaces/IOutliner.h"
#include "NxFundamental/ECS/BaseComponent.h"
#include "NxFundamental/ITagHolder.h"
#include "NxFundamental/Transformable.h"
#include "RawBackend/IDrawable.h"

namespace NX
{

    struct SceneState
    {
        std::string name;
        Transformable trans;
        Core::StringAtom assetType;
        Core::StringAtom referenceAsset;
        nlohmann::json typeSpecificData;
    };

    void to_json(nlohmann::json& j, const SceneState& v);
    void from_json(const nlohmann::json& j, SceneState& v);

    CLASS();
    class SceneObject :
        public BaseComponent,
        public Foundation::IOutliner,
        public Transformable,
        public RawBackend::IDrawable,
        public Foundation::IDataIO
    {
        ECS_DECL(SceneObject, NX::BaseComponent, NX::Transformable, Foundation::IOutliner,
                 RawBackend::IDrawable);

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
            swap(static_cast<Foundation::IOutliner&>(a), static_cast<Foundation::IOutliner&>(b));
            swap(static_cast<Transformable&>(a), static_cast<Transformable&>(b));
            swap(static_cast<RawBackend::IDrawable&>(a), static_cast<RawBackend::IDrawable&>(b));
        }

        [[nodiscard]] Tag getTags() const override;

        [[nodiscard]] virtual Core::StringAtom shortStringify() const;

        void recalculateMatrices(const glm::mat4& mat = glm::mat4(1.f)) override;

        void onPreDeserialize(AbstractComponent* obj) override;
        void onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs) override;
        void onPreSerialize(const AbstractComponent* obj) const override;
        void onPostSerialize(const AbstractComponent* obj,
                             const RLogsCollector& logs) const override;

        [[nodiscard]] bool hasReferencedAsset() const noexcept;
        [[nodiscard]] Core::StringAtom getReferencedAsset() const;
        void _setReferencedAsset(const Core::StringAtom& logicPath);

        [[nodiscard]] virtual nlohmann::json getTypeSpecificSceneDataAsJson() const;
        virtual void applyTypeSpecificSceneData(const nlohmann::json& data);

        virtual void onAddedToScene() {}

        void draw(BaseCamera& camera) final;

        [[nodiscard]] virtual glm::vec3 getGlobalPosition() const;
        [[nodiscard]] virtual glm::vec3 getGlobalRotation() const;

        [[nodiscard]] Core::StringAtom getCacheHash() const override { return "IDrawable"_atom; }

    protected:
        virtual void onDraw(BaseCamera& camera) {}

        void makeTransformableTreeDirty();
        void onInitialize() override;
        void onPreInitialize() override;
        void onOutlineStatusChange(bool newStatus) override;

        void onDirtyMatrix() override;

    private:
        Core::StringAtom _referencedAsset;
    };

} // namespace NX

#include "SceneObject.generated.h" // added by the code generator. Better don't move it.
