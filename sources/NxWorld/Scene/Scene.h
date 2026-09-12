// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Delegate.h"
#include "Grid.h"
#include "NxFundamental/Assets/ECSAsset.h"
#include "NxWorld/Assets/SkyboxAsset.h"
#include "SceneObject.h"

#include <vector>

namespace Core
{
    class Actor;

    CLASS();
    class Scene : public Foundation::IDataIO, public Foundation::BaseLog
    {
        R_FRIEND(Scene);

    public:
        using ObjectContainerT = std::vector<SceneObject::Ptr>;

        struct StreamData
        {
            StreamData() = delete;

            static constexpr const char* sceneObjects = "sceneObjects";
        };

    public:
        Scene() = default;
        ~Scene() override = default;
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

        void initialize();

        void tick(float timeDelta);

        void directDraw(NX::ShaderProgram* skyboxShader = nullptr);

        void setSceneName(StringAtom name);

        [[nodiscard]] const StringAtom& getSceneName() const noexcept;

        [[nodiscard]] const ObjectContainerT& getObjects() const noexcept { return _sceneObjects; }
        [[nodiscard]] ObjectContainerT& getObjects() noexcept { return _sceneObjects; }

        template<IsComponent T>
        [[nodiscard]] T::Ptr gerFirstOf();

        void addUniqueObjectToScene(SceneObject::Ptr object);
        void addObjectToScene(SceneObject::Ptr object);
        void addBlueprintObjectToScene(const WeakData<ECSAsset>& asset, const StringAtom& name);
        bool deleteFromScene(const StringAtom& name);
        bool deleteFromScene(const BaseComponent* obj);
        bool deleteFromSceneOrFromObject(BaseComponent* obj);
        void duplicateSceneObject(const BaseComponent* obj);
        [[nodiscard]] IntrusivePtr<SceneObject> find(const BaseComponent* obj);

        [[nodiscard]] nlohmann::json serialize() const;
        void deserialize(RResourceStream<RJsonResourceStream>& data);
        [[nodiscard]] std::filesystem::path getCacheDir() const override;
        [[nodiscard]] StringAtom getCacheHash() const override;
        [[nodiscard]] spdlog::logger* getLogger() const override;

        Delegate<void(SceneObject*)>::Ptr onObjectAdded = Delegate<void(SceneObject*)>::Create();

    public:
        Grid grid;
        NXSkybox skybox;

    protected:
        ObjectContainerT _sceneObjects;

        FIELD();
        StringAtom _sceneName = "Default";

    private:
        void internal_addObjectToScene(SceneObject* object);

    private:
        // TODO: get rid of it, deprecated
        std::vector<Actor*> _postDrawBuffer;

        uint32_t _uniqueCounterName = 0;
    };

    template<IsComponent T>
    typename T::Ptr Scene::gerFirstOf()
    {
        for (auto&& obj : _sceneObjects)
        {
            if (auto t = DynamicCast<T>(obj))
            {
                return t;
            }
        }

        return nullptr;
    }

} // namespace Core
#include "Scene.generated.h" // added by the code generator. Better don't move it.
