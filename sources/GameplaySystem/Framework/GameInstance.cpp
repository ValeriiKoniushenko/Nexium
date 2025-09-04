// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "GameInstance.h"

#include "Camera/Camera.h"
#include "Core/Timer.h"
#include "Editor/Windows/GameViewport.h"
#include "Graphics/Image.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Window.h"
#include "Misc/FPSCounter.h"
#include "assimp/Importer.hpp"

std::unique_ptr<Core::GameInstance> gGameInstance = nullptr;

namespace Core
{

    void GameInstance::initialize()
    {
#ifdef DEBUG
        spdlog::set_level(spdlog::level::trace);
#endif
        std::cout << std::fixed << std::setprecision(15);
        spdlog::set_pattern(_spdlogDefaultPatter.toStdString());

        //-------------------- WINDOW ---------------------
        _window = &GetWindow();
        _window->create(_defaultWindowName, _defaultWindowSize);
        _window->onResize.subscribe(
            [this](ISize2 newSize)
            {
                updateViewport();
            });

        //-------------------- SHADER MANAGER ---------------------
        shaderManager.loadShaders(_shaderPath);
        shaderManager.debugLog("Was loaded {} shaders."_f << shaderManager.countOfShaders());
        for (const auto& notLoadedShader : shaderManager.getFailedShaders())
        {
            shaderManager.warnLog(
                "Shader '{}' found but not loaded. It contains some error[s]. See above in the lo"_f
                << notLoadedShader);
        }
        onLoadShaders();

        //-------------------- MISC ---------------------
        renderToTextureObject.generate();
        gameEditor.initialize();

        if (auto* spectator = gameScene.createAndGetActor<Spectator>())
        {
            currentCamera = spectator->findFirstChildOf<BaseCamera>();
        }

        onInitializeReadCache();

        loadCoreResources();
        onInitializeFinish();

        gameLoop();

        onFinishWriteCache();
    }

    void GameInstance::onInitializeReadCache()
    {
        gameScene.readFromCache();
    }

    void GameInstance::onFinishWriteCache()
    {
        gameScene.writeToCache();
    }

    void GameInstance::gameLoop()
    {
        FPSCounter fps;
        fps.start();
        FStopwatch clock;

        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_STENCIL_TEST);

        const int clearBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;

        while (!_window->shouldClose())
        {
            clock.start();
            _window->pollEvent();

            if (const auto* wnd = gameEditor.getWindow<GameViewportEWC>(); wnd && wnd->isFocused())
            {
                gameScene.tick(world.timeDelta);
            }

            if (renderMode.cast() == RenderMode::GameOnly)
            {
                glClear(clearBits);
                if (currentCamera)
                {
                    onTick(world.timeDelta);
                }
            }
            else
            {
                glClear(clearBits);
                gameEditor.tick(world.timeDelta);
                if (currentCamera)
                {
                    renderToTextureObject.callMePreDraw();
                    glClear(clearBits);

                    onTick(world.timeDelta);
                    renderToTextureObject.callMeAfterDraw();
                }
            }

            _window->swapBuffers();
            fps.newFrameUpdate();
            world.timeDelta = clock.stop();
        }

        infoLog("Total FPS for this session: {}"_f << fps.getFPS());
    }

    void GameInstance::updateViewport()
    {
        if (renderMode.cast() == RenderMode::GameOnly)
        {
            _window->updateViewport(windowAspectRatio, ViewportMode::ZoomIn);
        }
        else
        {
            UpdateGlViewport(static_cast<FSize2>(renderToTextureObject.getRenderSize()),
                             windowAspectRatio, ViewportMode::ZoomIn);
        }
    }

    void GameInstance::toggleRenderMode()
    {
        using R = RenderMode;
        renderMode = renderMode.cast() == R::GameOnly ? R::Editor : R::GameOnly;
        gGameInstance->updateViewport();
    }

    void GameInstance::onLoadShaders()
    {
        auto* colorShader = shaderManager.getShaderProgram("color"_atom);
        if (Verify(colorShader))
        {
            colorShader->setVertexAttributeCallback(
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

        auto* simpleColorShader = shaderManager.getShaderProgram("simple_color"_atom);
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
    }

    void GameInstance::loadCoreResources()
    {
    }
} // namespace Core