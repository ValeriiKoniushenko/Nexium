// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "GameInstance.h"

#include "Core/Size.h"
#include "Foundation/Configs.h"
#include "Foundation/Debug/Latency.h"
#include "InputSystem.h"
#include "NxSubsystems/Graphics/ShaderManager.h"
#include "NxWorld/Animations/FrameByFrame/FrameByFrameAnimation.h"
#include "NxWorld/Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/PrivateModuleInfo.h"
#include "NxWorld/Scene/SceneObjects/Rectangle/Rectangle.h"
#include "Platform/Glfw.h"
#include "Platform/Window.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"

std::unique_ptr<NX::GameInstance> gGameInstance = nullptr;

namespace NX
{

    World* GetWorld()
    {
        if (gGameInstance) [[likely]]
        {
            return &gGameInstance->world;
        }
        return nullptr;
    }

    AssetsManager* GetAssetsManager()
    {
        if (gGameInstance) [[likely]]
        {
            return &gGameInstance->assets;
        }
        return nullptr;
    }

    Scene* GetGameScene()
    {
        return gGameInstance ? gGameInstance->scenes.getCurrentScene() : nullptr;
    }

    SceneManager* GetSceneManager()
    {
        return gGameInstance ? &gGameInstance->scenes : nullptr;
    }

    bool IsEditorMode()
    {
        return gGameInstance && gGameInstance->renderMode == GameInstance::RenderMode::Editor;
    }

    void ResetCamera()
    {
        gGameInstance->resetCamera();
    }

    void SaveAllToCache()
    {
        gGameInstance->saveAllToCache();
    }

    GameInstance::GameInstance(int argc, char** argv)
    {
        NX_LATENCY_POINT("Game start");

        if (argc == 1)
        {
            return;
        }

        if (argc == 3 && std::string(argv[1]) == "--timeout")
        {
            _timeout = std::stof(argv[2]);
        }
        else
        {
            const char* str
                = "Invalid arguments for the game instance. Expected: --timeout <timeout in s>";
            Assert(false, str);
            errorLog(str);
        }
    }

    spdlog::logger* GameInstance::getLogger() const
    {
        return NxWorld::getLogger();
    }

    void GameInstance::setApplicationIntegration(ApplicationIntegration* integration) noexcept
    {
        _applicationIntegration = integration;
        if (!integration && renderMode == RenderMode::Editor)
        {
            renderMode = RenderMode::GameOnly;
        }
    }

    bool GameInstance::isEditorMode() const noexcept
    {
        return renderMode == RenderMode::Editor && _applicationIntegration != nullptr;
    }

    bool GameInstance::isApplicationViewportFocused() const
    {
        return !isEditorMode() || _applicationIntegration->isViewportFocused();
    }

    Core::ISize2 GameInstance::getRenderSize() const
    {
        if (isEditorMode())
        {
            return _applicationIntegration->getRenderSize();
        }

        return window ? window->getSize() : Core::ISize2{};
    }

    void GameInstance::initialize()
    {
#ifdef NEXIUM_DEBUG
        spdlog::set_level(spdlog::level::debug);
#endif
        std::cout << std::fixed << std::setprecision(15);
        spdlog::set_pattern(Foundation::Config::spdlogPattern);

        NX_LATENCY_POINT("start");
        //-------------------- WINDOW ---------------------
        window = &Platform::GetWindow();
        window->create(Foundation::Config::defaultWindowName,
                       Foundation::Config::defaultWindowSize);
        GetInputSystem().initialize(*window);
        _subscriptionPool << window->onResize->subscribeAndGetID([this](Core::ISize2 newSize)
                                                                 { updateViewport(); });
        glfwSetFramebufferSizeCallback(window->getRawWindow(), [](GLFWwindow*, int, int)
                                       { gGameInstance->updateViewport(); });

        NX_LATENCY_POINT("Window - inited");

        //-------------------- ASSETS MANAGER ---------------------
        GetAssetsManager()->initScanFileSystem();

        NX_LATENCY_POINT("AssetsManager - inited");

        //-------------------- SHADER MANAGER ---------------------
        GetShaderManager().loadShaders(Foundation::Config::Path::shaders);
        GetShaderManager().debugLog("Was loaded {} shaders."_f
                                    << GetShaderManager().countOfShaders());
        for (const auto& notLoadedShader : GetShaderManager().getFailedShaders())
        {
            GetShaderManager().criticalLog(
                "Shader '{}' found but not loaded. It contains some error[s]. See above in the log"_f
                << notLoadedShader);
        }
        initializeShaders();

        NX_LATENCY_POINT("ShaderManager - inited");

        //-------------------- ECS ---------------------
        if (_applicationIntegration)
        {
            _applicationIntegration->preInitialize();
        }
        GetGlobalComponentFactory().initializeTypeToTagMap();
        if (_applicationIntegration)
        {
            _applicationIntegration->initialize();
        }

        NX_LATENCY_POINT("ECS - inited");
        gGameInstance->scenes.getCurrentScene()->initialize();
        _subscriptionPool << scenes.onCurrentSceneChanged->subscribeAndGetID(
            [this](Scene* scene) { bindCurrentScene(scene); });
        bindCurrentScene(scenes.getCurrentScene());
        NX_LATENCY_POINT("Scene - inited");

        startUpReadCache();
        loadCoreResources();

        NX_LATENCY_POINT("Resources - inited");

        NX_LATENCY_POINT("end");
    }

    void GameInstance::bindCurrentScene(Scene* scene)
    {
        _sceneSubscriptionPool.clearAndReleaseAll();
        resetCamera();
        _sceneSubscriptionPool << scene->onObjectAdded->subscribeAndGetID(
            [this](SceneObject* obj) { internal_onAddObjectToScene(obj); });
        for (const auto& object : scene->getObjects())
        {
            internal_onAddObjectToScene(object.get());
        }
    }

    void GameInstance::startUpReadCache()
    {
        if (_applicationIntegration)
        {
            _applicationIntegration->readFromCache();
        }
        GetCacheSystem().tryRead(Platform::GetWindow());
        try
        {
            scenes.importScenes(Foundation::Config::Path::data / "scenes");
            GetCacheSystem().tryRead(scenes);
        }
        catch (const std::exception& error)
        {
            _canSaveSceneLibrary = false;
            errorLog("Cannot load scene files: {}"_f << error.what());
        }
        GetCacheSystem().tryRead(*GetWorld());
        onInitializeReadCache();
    }

    void GameInstance::saveAllToCache()
    {
        if (_applicationIntegration)
        {
            _applicationIntegration->writeToCache();
        }

        GetCacheSystem().write(*GetWorld());
        if (_canSaveSceneLibrary)
        {
            for (const auto& scene : scenes.getScenes())
            {
                GetCacheSystem().write(*scene);
            }
            GetCacheSystem().write(scenes);
        }
        GetCacheSystem().write(Platform::GetWindow());

        onSaveAll();
    }

    void GameInstance::resetCamera()
    {
        world.currentCamera = nullptr;
        if (_applicationIntegration)
        {
            _applicationIntegration->clearSceneRenderTarget();
        }
    }

    void GameInstance::updateViewport()
    {
        if (!isEditorMode())
        {
            window->updateViewport();
        }
        else
        {
            const auto size = _applicationIntegration->getRenderSize();
            glViewport(0, 0, size.width, size.height);
        }

        if (auto* world = GetWorld())
        {
            if (world->currentCamera)
            {
                world->currentCamera->invalidateCameraMatrices();
            }
        }
    }

    void GameInstance::toggleRenderMode()
    {
        using R = RenderMode;
        renderMode = renderMode == R::GameOnly && _applicationIntegration ? R::Editor : R::GameOnly;
        gGameInstance->updateViewport();
    }

    void GameInstance::initializeShaders()
    {
        auto* defaultShader = GetShaderManager().getShaderProgram("defaultTextured"_atom);
        if (Verify(defaultShader))
        {
            defaultShader->setVertexAttributeCallback(
                []
                {
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);

                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                                          reinterpret_cast<void*>(3 * sizeof(float)));

                    glEnableVertexAttribArray(2);
                    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                                          reinterpret_cast<void*>(6 * sizeof(float)));
                });
        }

        auto* outlineShader = GetShaderManager().getShaderProgram("outline"_atom);
        if (Verify(outlineShader))
        {
            outlineShader->setVertexAttributeCallback(
                []
                {
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);

                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                                          reinterpret_cast<void*>(3 * sizeof(float)));
                });
        }

        auto* objectIdentifierShader = GetShaderManager().getShaderProgram("objectIdentifier"_atom);
        if (Verify(objectIdentifierShader))
        {
            objectIdentifierShader->setVertexAttributeCallback(
                []
                {
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
                });
        }

        auto* simpleColorShader = GetShaderManager().getShaderProgram("pickUpColorFiller"_atom);
        if (Verify(simpleColorShader))
        {
            simpleColorShader->setVertexAttributeCallback(
                []
                {
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);

                    glEnableVertexAttribArray(2);
                    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                                          reinterpret_cast<void*>(6 * sizeof(float)));
                });
        }

        auto* skyboxShader = GetShaderManager().getShaderProgram("skybox"_atom);
        if (Verify(skyboxShader))
        {
            skyboxShader->setVertexAttributeCallback(
                []
                {
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
                });
        }

        auto* main2dShader = GetShaderManager().getShaderProgram("2d_rect"_atom);
        if (Verify(main2dShader))
        {
            main2dShader->setSetEventCallback(
                [](ShaderProgram::Event event)
                {
                    if (event == ShaderProgram::Event::OnSetIndexAndVertexBuffer)
                    {
                        glEnableVertexAttribArray(0);
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
                    }
                    else if (event == ShaderProgram::Event::OnSetTextureVertexBuffer)
                    {
                        glEnableVertexAttribArray(1);
                        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
                    }
                });
        }

        auto* lineShader = GetShaderManager().getShaderProgram("line"_atom);
        if (Verify(lineShader))
        {
            lineShader->setSetEventCallback(
                [](ShaderProgram::Event event)
                {
                    if (event == ShaderProgram::Event::OnSetIndexAndVertexBuffer)
                    {
                        glEnableVertexAttribArray(0);
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
                    }
                });
        }

        onLoadShaders();
    }

    Core::StringAtom GameInstance::getCacheHash() const
    {
        return "GameInstance"_atom;
    }

    void GameInstance::internal_onAddObjectToScene(SceneObject* obj)
    {
        if (!world.currentCamera)
        {
            obj->forEach(
                [this](BaseComponent* obj)
                {
                    if (auto* camera = dynamic_cast<BaseCamera*>(obj))
                    {
                        world.currentCamera = camera;
                        return false;
                    }
                    return true;
                });
        }
    }

    void GameInstance::loadCoreResources()
    {
        GetAssetsManager()->generateTextureAtlas(Foundation::Config::Path::images / "atlas");
        GetAssetsManager()->generateTextureAtlas("santa_walk"_atom, Foundation::Config::Path::images
                                                                        / "Santa/Santa_Walk");
        GetAssetsManager()->generateTextureAtlas(
            "player_walk"_atom, Foundation::Config::Path::images / "Player_SpriteSheet");
        onLoadCoreResources();
    }
} // namespace NX
