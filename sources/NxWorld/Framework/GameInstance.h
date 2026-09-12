// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ApplicationIntegration.h"
#include "NxSubsystems/AssetsManager/AssetsManager.h"
#include "NxSubsystems/Graphics/ShaderManager.h"
#include "NxWorld/Scene/Scene.h"
#include "Platform/Window.h"
#include "UserInterface.h"
#include "World.h"

namespace Core
{
    class GameInstance : public Foundation::BaseLog, public Foundation::IDataIO
    {
    public:
        ENUM_CLASS();
        enum class RenderMode
        {
            GameOnly,
            Editor
        };

    public:
        GameInstance(int argc, char** argv);
        GameInstance(const GameInstance&) = delete;
        GameInstance(GameInstance&&) = delete;
        GameInstance& operator=(const GameInstance&) = delete;
        GameInstance& operator=(GameInstance&&) = delete;
        ~GameInstance() override = default;

        [[nodiscard]] spdlog::logger* getLogger() const override;
        [[nodiscard]] const char* getPrefix() const override { return "GameInstance"; }

        void initialize();

        void updateViewport();

        void toggleRenderMode();

        void saveAllToCache();

        void setApplicationIntegration(ApplicationIntegration* integration) noexcept;
        [[nodiscard]] ApplicationIntegration* getApplicationIntegration() const noexcept
        {
            return _applicationIntegration;
        }
        [[nodiscard]] bool isEditorMode() const noexcept;
        [[nodiscard]] bool isApplicationViewportFocused() const;
        [[nodiscard]] ISize2 getRenderSize() const;

    public:
        Scene gameScene;
        ShaderManager shaderManager;
        World world;
        UserInterface userInterface;
        AssetsManager assets;

        void resetCamera();
        RenderMode renderMode = RenderMode::Editor;
        Window* window = nullptr;

        [[nodiscard]] StringAtom getCacheHash() const override;

    protected:
        virtual void onSaveAll() {}
        virtual void onTick(float delta) {}
        virtual void onLoadShaders() {}
        virtual void onLoadCoreResources() {}
        virtual void onInitializeReadCache() {}
        void internal_onAddObjectToScene(SceneObject* obj);

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;

    private:
        void loadCoreResources();
        void startUpReadCache();
        void initializeShaders();
        void runMainLoop();

    private:
        ApplicationIntegration* _applicationIntegration = nullptr;

        // Timeout in seconds. 0 - no timeout
        float _timeout = 0;
    };

    [[nodiscard]] World* GetWorld();

    [[nodiscard]] AssetsManager* GetAssetsManager();
} // namespace Core

extern std::unique_ptr<Core::GameInstance> gGameInstance;
#include "GameInstance.generated.h" // added by the code generator. Better don't move it.
