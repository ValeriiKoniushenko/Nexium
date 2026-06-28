/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "GameInstance.h"

#include "../Camera.h"
#include "Core/Timer.h"
#include "Editor/Windows/GameViewport.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Window.h"
#include "ImGui/imgui.h"
#include "Misc/Configs.h"
#include "Misc/FPSCounter.h"
#include "ModuleInfo.h"
#include "ResourceManagement/ResourceManager.h"
#include "Scene/Rectangle.h"
#include "Scene/Spectator.h"
#include "spdlog/spdlog.h"

std::unique_ptr<Core::GameInstance> gGameInstance = nullptr;

namespace Core
{
    ShaderManager& GetShaderManager()
    {
        return gGameInstance->shaderManager;
    }

    GameEditor& GetEditor()
    {
        return gGameInstance->gameEditor;
    }

    World& GetWorld()
    {
        return gGameInstance->world;
    }

    AssetsManager& GetAssetsManager()
    {
        return gGameInstance->assets;
    }

    spdlog::logger* GameInstance::getLogger() const
    {
        return Framework::getLogger();
    }

    void GameInstance::initialize()
    {
#ifdef DEBUG
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
        GetAssetsManager().initScanFileSystem();

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

        //-------------------- MISC ---------------------
        gameViewport.generate();

        gameEditor.initialize();
        gameScene.initialize();

        startUpReadCache();
        loadCoreResources();

        runMainLoop();

        saveAll();
    }

    void GameInstance::startUpReadCache()
    {
        gameEditor.readFromCache();
        GetCacheSystem().tryRead(gameScene);

        // world.tryReadFromCache();
        // gameScene.tryReadFromCache();
        onInitializeReadCache();

        ImGui::LoadIniSettingsFromDisk(Config::Path::imGuiWindowsIni.generic_string().c_str());
    }

    void GameInstance::saveAll()
    {
        gameEditor.writeToCache();
        // world.writeToCache();
        ImGui::SaveIniSettingsToDisk(Config::Path::imGuiWindowsIni.generic_string().c_str());

        GetCacheSystem().write(gameScene);

        onSaveAll();
    }

    void GameInstance::runMainLoop()
    {
        FPSCounter fps;
        fps.start();
        FStopwatch clock;

        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_STENCIL_TEST);

        constexpr int clearBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;

        while (!window->shouldClose())
        {
            clock.start();
            Window::pollEvent();

            if (renderMode == RenderMode::GameOnly)
            {
                gameScene.tick(world.timeDelta);

                glClear(clearBits);

                gameEditor.keyboardInput.update(); // force update

                if (currentCamera)
                {
                    gameScene.directDraw();
                    onTick(world.timeDelta);
                }
            }
            else
            {
                if (const auto* wnd = gameEditor.getWindow<GameViewportEWC>();
                    wnd && wnd->isFocused())
                {
                    gameScene.tick(world.timeDelta);
                }

                glClear(clearBits);
                gameEditor.tick(world.timeDelta);
                if (currentCamera)
                {
                    gameViewport.callMePreDraw();
                    glClear(clearBits);

                    gameScene.directDraw();
                    onTick(world.timeDelta);
                    gameViewport.callMeAfterDraw();
                }
            }

            if (glfwGetWindowAttrib(window->getRawWindow(), GLFW_ICONIFIED)
                || glfwGetWindowAttrib(window->getRawWindow(), GLFW_FOCUSED) == GLFW_FALSE)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            window->swapBuffers();
            fps.newFrameUpdate();
            world.timeDelta = clock.stop();
        }

        infoLog("Total FPS for this session: {}"_f << fps.getFPS());
    }

    void GameInstance::updateViewport()
    {
        if (renderMode == RenderMode::GameOnly)
        {
            window->updateViewport(Config::windowAspectRatio, ViewportMode::ZoomIn);
        }
        else
        {
            UpdateGlViewport(static_cast<FSize2>(gameViewport.getRenderSize()),
                             Config::windowAspectRatio, ViewportMode::ZoomIn);
        }
    }

    void GameInstance::toggleRenderMode()
    {
        using R = RenderMode;
        renderMode = renderMode == R::GameOnly ? R::Editor : R::GameOnly;
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

        onLoadShaders();
    }

    StringAtom GameInstance::getCacheHash() const
    {
        return "GameInstance"_atom;
    }

    void GameInstance::loadCoreResources()
    {
        GetAssetsManager().generateTextureAtlas(Config::Path::images / "atlas");

        gameScene._sceneObjects.emplace_back(SceneObj::Rectangle::Create());

        onLoadCoreResources();
    }
} // namespace Core