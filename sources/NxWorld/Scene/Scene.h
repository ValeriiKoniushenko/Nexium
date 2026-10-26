// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::World

#pragma once

#include "Core/Delegate.h"
#include "Core/IntrusivePtr.h"
#include "NxFundamental/Assets/ECSAsset.h"
#include "NxSubsystems/Graphics/Font.h"
#include "NxWorld/Assets/SkyboxAsset.h"
#include "NxWorld/Scene/SceneObjects/Grid/Grid.h"
#include "NxWorld/Scene/SceneObjects/SceneObject.h"

#include <vector>

namespace NX
{
    class Actor;
    class BaseCamera;
    class SceneManager;
    class World;

    CLASS();
    class Scene :
        public Foundation::IDataIO,
        public Foundation::BaseLog,
        public Core::IntrusiveRefCounter<Scene>
    {
        R_FRIEND(Scene);
        // Only the manager decides which scene is current, so only it activates scenes.
        friend class SceneManager;

    public:
        using ObjectContainerT = std::vector<SceneObject::Ptr>;

        struct StreamData
        {
            StreamData() = delete;

            static constexpr const char* sceneObjects = "sceneObjects";
        };

    public:
        Scene() = default;
        ~Scene() override;
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

        void initialize();
        void deinitialize();
        [[nodiscard]] bool isInitialized() const noexcept { return _isInitialized; }
        [[nodiscard]] bool isActive() const noexcept { return _isActive; }

        // Remembers the camera this scene renders with; it is restored whenever the scene
        // becomes current again. The camera must belong to one of the scene's objects.
        void setMainCamera(BaseCamera* camera);

        void tick(float timeDelta);

        void directDraw(NX::ShaderProgram* skyboxShader = nullptr,
                        NX::ShaderProgram* gridShader = nullptr);

        void setSceneName(Core::StringAtom name);

        [[nodiscard]] const Core::StringAtom& getSceneName() const noexcept;

        [[nodiscard]] const ObjectContainerT& getObjects() const noexcept { return _sceneObjects; }
        [[nodiscard]] ObjectContainerT& getObjects() noexcept { return _sceneObjects; }

        template<IsComponent T>
        [[nodiscard]] T::Ptr gerFirstOf();

        void addUniqueObjectToScene(SceneObject::Ptr object);
        void addObjectToScene(SceneObject::Ptr object);
        void addBlueprintObjectToScene(const Core::WeakData<ECSAsset>& asset,
                                       const Core::StringAtom& name);
        bool deleteFromScene(const Core::StringAtom& name);
        bool deleteFromScene(const BaseComponent* obj);
        bool deleteFromSceneOrFromObject(BaseComponent* obj);
        void duplicateSceneObject(const BaseComponent* obj);
        [[nodiscard]] Core::IntrusivePtr<SceneObject> find(const BaseComponent* obj);

        [[nodiscard]] nlohmann::json serialize() const;
        void deserialize(RResourceStream<RJsonResourceStream>& data);
        [[nodiscard]] std::filesystem::path getCacheDir() const override;
        [[nodiscard]] Core::StringAtom getCacheHash() const override;
        [[nodiscard]] spdlog::logger* getLogger() const override;

        Core::Delegate<void(SceneObject*)>::Ptr onObjectAdded
            = Core::Delegate<void(SceneObject*)>::Create();

    public:
        Grid grid;
        NXSkybox skybox;

    protected:
        ObjectContainerT _sceneObjects;

        FIELD();
        Core::StringAtom _sceneName = "Default";

    private:
        // Makes this the scene rendered by `world` (may be null, e.g. in tests) and restores its
        // main camera there. Deactivation stores the camera in use and clears it from the world.
        void activate(World* world);
        void deactivate();
        void internal_addObjectToScene(SceneObject* object);
        void selectCameraFrom(SceneObject& object);
        [[nodiscard]] bool containsComponent(const BaseComponent* component) const;

    private:
        // Holds a BaseCamera; the base type keeps Camera.h out of this header.
        Core::WeakPtr<BaseComponent> _mainCamera;

        // TODO: get rid of it, deprecated
        std::vector<Actor*> _postDrawBuffer;

        NX::Face nxFace;
        // Generated on the first draw: scenes may initialize before shaders are loaded.
        bool _isFaceGenerated = false;

        uint32_t _uniqueCounterName = 0;
        bool _isInitialized = false;
        bool _isActive = false;
        // The world this scene renders into while it is active.
        World* _world = nullptr;
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

} // namespace NX
#include "Scene.generated.h"
