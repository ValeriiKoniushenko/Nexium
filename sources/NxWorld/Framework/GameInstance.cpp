// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "GameInstance.h"

#include "Animations/FrameByFrame/FrameByFrameAnimation.h"
#include "Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "Core/Timer.h"
#include "Foundation/Configs.h"
#include "Misc/FPSCounter.h"
#include "Platform/Glfw.h"
#include "PrivateModuleInfo.h"
#include "ResourceManagement/ResourceManager.h"
#include "Scene/Rectangle.h"
#include "spdlog/spdlog.h"

std::unique_ptr<Core::GameInstance> gGameInstance = nullptr;

namespace Core
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

    ISize2 GameInstance::getRenderSize() const
    {
        if (isEditorMode())
        {
            return _applicationIntegration->getRenderSize();
        }

        return window ? window->getSize() : ISize2{};
    }

    void GameInstance::initialize()
    {
#ifdef NEXIUM_DEBUG
        spdlog::set_level(spdlog::level::trace);
#endif
        std::cout << std::fixed << std::setprecision(15);
        spdlog::set_pattern(Config::spdlogPattern);

        //-------------------- WINDOW ---------------------
        window = &GetWindow();
        window->create(Config::defaultWindowName, Config::defaultWindowSize);
        _subscriptionPool << window->onResize->subscribeAndGetID([this](ISize2 newSize)
                                                                 { updateViewport(); });

        //-------------------- ASSETS MANAGER ---------------------
        GetAssetsManager()->initScanFileSystem();

        //-------------------- SHADER MANAGER ---------------------
        shaderManager.loadShaders(Config::Path::shaders);
        shaderManager.debugLog("Was loaded {} shaders."_f << shaderManager.countOfShaders());
        for (const auto& notLoadedShader : shaderManager.getFailedShaders())
        {
            shaderManager.criticalLog(
                "Shader '{}' found but not loaded. It contains some error[s]. See above in the log"_f
                << notLoadedShader);
        }
        initializeShaders();

        //-------------------- ECS ---------------------
        GetGlobalComponentFactory()._createTypeToTagMap();

        if (_applicationIntegration)
        {
            _applicationIntegration->initialize();
        }
        gameScene.initialize();
        _subscriptionPool << gameScene.onObjectAdded->subscribeAndGetID(
            [this](SceneObject* obj) { internal_onAddObjectToScene(obj); });

        startUpReadCache();
        loadCoreResources();

        runMainLoop();

        saveAllToCache();
    }

    void GameInstance::startUpReadCache()
    {
        if (_applicationIntegration)
        {
            _applicationIntegration->readFromCache();
        }
        GetCacheSystem().tryRead(gameScene);
        onInitializeReadCache();
    }

    void GameInstance::saveAllToCache()
    {
        if (_applicationIntegration)
        {
            _applicationIntegration->writeToCache();
        }
        // world.writeToCache();

        GetCacheSystem().write(gameScene);

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

    void GameInstance::runMainLoop()
    {
        FPSCounter fps;
        fps.start();
        FStopwatch clock;

        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_STENCIL_TEST);

        constexpr int clearBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;

        while (!window->shouldClose())
        {
            clock.start();
            Window::pollEvent();

            if (!isEditorMode())
            {
                gameScene.tick(world.getTimeDelta());

                glClear(clearBits);

                if (_applicationIntegration)
                {
                    _applicationIntegration->updateInput();
                }

                if (world.currentCamera)
                {
                    gameScene.directDraw(shaderManager.getShaderProgram("skybox"_atom));
                    onTick(world.getTimeDelta());
                }
            }
            else
            {
                if (_applicationIntegration->isViewportFocused())
                {
                    gameScene.tick(world.getTimeDelta());
                }

                glClear(clearBits);
                _applicationIntegration->tick(world.getTimeDelta());

                if (world.currentCamera)
                {
                    _applicationIntegration->updateSceneInteraction(gameScene);
                    _applicationIntegration->beforeSceneDraw();
                    glClear(clearBits);

                    gameScene.directDraw(shaderManager.getShaderProgram("skybox"_atom));
                    onTick(world.getTimeDelta());
                    _applicationIntegration->afterSceneDraw();
                }
            }

            if (glfwGetWindowAttrib(window->getRawWindow(), GLFW_ICONIFIED)
                || glfwGetWindowAttrib(window->getRawWindow(), GLFW_FOCUSED) == GLFW_FALSE)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            if (_timeout != 0.f)
            {
                if (world.getWorldTime() > _timeout)
                {
                    window->close();
                    infoLog("Force closing the window due to the passed timeout ({} seconds)."_f
                            << _timeout);
                }
            }

            window->swapBuffers();
            fps.newFrameUpdate();
            world.internal_UpdateTimeDelta(clock.stop());
        }

        infoLog("Total FPS for this session: {}"_f << fps.getFPS());
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

    StringAtom GameInstance::getCacheHash() const
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
        GetAssetsManager()->generateTextureAtlas(Config::Path::images / "atlas");
        GetAssetsManager()->generateTextureAtlas("santa_walk"_atom,
                                                 Config::Path::images / "Santa/Santa_Walk");
        GetAssetsManager()->generateTextureAtlas("player_walk"_atom,
                                                 Config::Path::images / "Player_SpriteSheet");
        onLoadCoreResources();
    }
} // namespace Core
