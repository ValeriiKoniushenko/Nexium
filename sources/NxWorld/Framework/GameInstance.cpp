// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "GameInstance.h"

#include "../Scene/SceneObjects/Rectangle/Rectangle.h"
#include "Core/Size.h"
#include "Foundation/Configs.h"
#include "NxWorld/Animations/FrameByFrame/FrameByFrameAnimation.h"
#include "NxWorld/Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/PrivateModuleInfo.h"
#include "Platform/Glfw.h"
#include "Platform/Window.h"
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

    SceneManager* GetSceneManager()
    {
        if (gGameInstance) [[likely]]
        {
            return &gGameInstance->scenes;
        }
        return nullptr;
    }

    GameInstance::GameInstance(int argc, char** argv)
    {
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
        spdlog::set_level(spdlog::level::trace);
#endif
        std::cout << std::fixed << std::setprecision(15);
        spdlog::set_pattern(Foundation::Config::spdlogPattern);

        //-------------------- WINDOW ---------------------
        window = &Platform::GetWindow();
        window->create(Foundation::Config::defaultWindowName,
                       Foundation::Config::defaultWindowSize);
        _subscriptionPool << window->onResize->subscribeAndGetID([this](Core::ISize2 newSize)
                                                                 { updateViewport(); });

        //-------------------- ASSETS MANAGER ---------------------
        GetAssetsManager()->initScanFileSystem();

        //-------------------- SHADER MANAGER ---------------------
        shaderManager.loadShaders(Foundation::Config::Path::shaders);
        shaderManager.debugLog("Was loaded {} shaders."_f << shaderManager.countOfShaders());
        for (const auto& notLoadedShader : shaderManager.getFailedShaders())
        {
            shaderManager.criticalLog(
                "Shader '{}' found but not loaded. It contains some error[s]. See above in the log"_f
                << notLoadedShader);
        }
        initializeShaders();

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

        gGameInstance->scenes.getCurrentScene()->initialize();
        _subscriptionPool
            << gGameInstance->scenes.getCurrentScene()->onObjectAdded->subscribeAndGetID(
                   [this](SceneObject* obj) { internal_onAddObjectToScene(obj); });

        startUpReadCache();
        loadCoreResources();
    }

    void GameInstance::startUpReadCache()
    {
        if (_applicationIntegration)
        {
            _applicationIntegration->readFromCache();
        }
        GetCacheSystem().tryRead(Platform::GetWindow());
        GetCacheSystem().tryRead(*gGameInstance->scenes.getCurrentScene());
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
        GetCacheSystem().write(*gGameInstance->scenes.getCurrentScene());
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
            const auto size = window->getSize();
            glViewport(0, 0, size.width, size.height);
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
        auto* defaultShader = shaderManager.getShaderProgram("defaultTextured"_atom);
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

        auto* outlineShader = shaderManager.getShaderProgram("outline"_atom);
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

        auto* objectIdentifierShader = shaderManager.getShaderProgram("objectIdentifier"_atom);
        if (Verify(objectIdentifierShader))
        {
            objectIdentifierShader->setVertexAttributeCallback(
                []
                {
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
                });
        }

        auto* simpleColorShader = shaderManager.getShaderProgram("pickUpColorFiller"_atom);
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

        auto* skyboxShader = shaderManager.getShaderProgram("skybox"_atom);
        if (Verify(skyboxShader))
        {
            skyboxShader->setVertexAttributeCallback(
                []
                {
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
                });
        }

        auto* main2dShader = shaderManager.getShaderProgram("2d_rect"_atom);
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

        auto* lineShader = shaderManager.getShaderProgram("line"_atom);
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
