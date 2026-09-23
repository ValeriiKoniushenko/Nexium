// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Runtime.h"

#include "Core/Timer.h"
#include "NxRuntime/GameUtils/FPSCounter.h"
#include "NxWorld/Framework/GameInstance.h"
#include "Platform/Glfw.h"

#include <chrono>
#include <thread>

namespace NX
{
    Runtime::Runtime(GameInstance& gameInstance) noexcept
        : _gameInstance(gameInstance)
    {
    }

    void Runtime::run()
    {
        _gameInstance.initialize();
        runMainLoop();
        _gameInstance.saveAllToCache();
    }

    GameInstance& Runtime::getGameInstance() const noexcept
    {
        return _gameInstance;
    }

    void Runtime::runMainLoop()
    {
        NX::FPSCounter fps;
        fps.start();
        Core::FStopwatch clock;

        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_STENCIL_TEST);

        constexpr int clearBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;

        while (!_gameInstance.window->shouldClose())
        {
            clock.start();
            Platform::Window::pollEvent();

            if (!_gameInstance.isEditorMode())
            {
                _gameInstance.scenes.getCurrentScene()->tick(_gameInstance.world.getTimeDelta());

                glClear(clearBits);

                if (auto* integration = _gameInstance.getApplicationIntegration())
                {
                    integration->updateInput();
                }

                if (_gameInstance.world.currentCamera)
                {
                    _gameInstance.scenes.getCurrentScene()->directDraw(
                        _gameInstance.shaderManager.getShaderProgram("skybox"_atom),
                        _gameInstance.shaderManager.getShaderProgram("grid"_atom));
                    _gameInstance.tick(_gameInstance.world.getTimeDelta());
                }
            }
            else
            {
                auto* integration = _gameInstance.getApplicationIntegration();
                if (integration->isViewportFocused())
                {
                    _gameInstance.scenes.getCurrentScene()->tick(
                        _gameInstance.world.getTimeDelta());
                }

                glClear(clearBits);
                integration->tick(_gameInstance.world.getTimeDelta());

                if (_gameInstance.world.currentCamera)
                {
                    integration->updateSceneInteraction(*_gameInstance.scenes.getCurrentScene());
                    integration->beforeSceneDraw();
                    glClear(clearBits);

                    _gameInstance.scenes.getCurrentScene()->directDraw(
                        _gameInstance.shaderManager.getShaderProgram("skybox"_atom),
                        _gameInstance.shaderManager.getShaderProgram("grid"_atom));
                    _gameInstance.tick(_gameInstance.world.getTimeDelta());
                    integration->afterSceneDraw();
                }
            }

            if (glfwGetWindowAttrib(_gameInstance.window->getRawWindow(), GLFW_ICONIFIED)
                || glfwGetWindowAttrib(_gameInstance.window->getRawWindow(), GLFW_FOCUSED)
                       == GLFW_FALSE)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            if (_gameInstance.getTimeout() != 0.f
                && _gameInstance.world.getWorldTime() > _gameInstance.getTimeout())
            {
                _gameInstance.window->close();
                _gameInstance.infoLog(
                    "Force closing the window due to the passed timeout ({} seconds)."_f
                    << _gameInstance.getTimeout());
            }

            _gameInstance.window->swapBuffers();
            fps.newFrameUpdate();
            _gameInstance.world.internal_UpdateTimeDelta(clock.stop());
        }

        _gameInstance.infoLog("Total FPS for this session: {}"_f << fps.getFPS());
    }
} // namespace NX
