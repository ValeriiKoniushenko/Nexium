/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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
#include "Editor/Windows/ModalECSSearchPopUp.h"
#include "Editor/Windows/NxECSBasedEditor.h"
#include "Graphics/Image.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Window.h"
#include "Misc/FPSCounter.h"
#include "ModuleInfo.h"
#include "Rectpack2D/finders_interface.h"
#include "Scene/Spectator.h"
#include "assimp/Importer.hpp"
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

        // tryReadFromCache();

        //-------------------- WINDOW ---------------------
        window = &GetWindow();
        window->create(Config::defaultWindowName, Config::defaultWindowSize);
        _subscriptionPool << window->onResize->subscribeAndGetID([this](ISize2 newSize)
                                                                 { updateViewport(); });

        //-------------------- ASSETS MANAGER ---------------------
        GetAssetsManager().rescanFileSystem();

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

        if (auto spectator = gameScene.createAndGet<Spectator>())
        {
            currentCamera = spectator->findFirstChildOf<BaseCamera>();
        }

        gameEditor.initialize();
        gameScene.initialize();

        /*Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            "/home/valerii/workspace/Nexium/assets/objects-3d/Models/FBX/FireHydrant.fbx",
            aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_JoinIdenticalVertices);

        StaticMeshBundle b;
        b.setScale({ 2, 2, 2 });
        b.setOrigin({ 0, 0, 100 });
        b.setPosition({ 200, 0, 0 });
        b.setComponentName("MyMesh");
        b.setIsPostDraw(true);

        if (Verify(scene) && Verify(scene->mRootNode))
        {
            b.importFrom(
                scene->mRootNode, scene,
                "/home/valerii/workspace/Nexium/assets/objects-3d/Models/FBX/FireHydrant.fbx", 1);
        }

        DataStream stream;
        stream.setMode(DataStream::Mode::Output);
        b.ioFieldsUpdate(stream);
        std::cout << stream.getRaw().dump(4) << std::endl;*/
        startUpReadCache();
        loadCoreResources();

        runMainLoop();

        saveAll();
    }

    void GameInstance::startUpReadCache()
    {
        gameEditor.readFromCache();
        // world.tryReadFromCache();
        // gameScene.tryReadFromCache();
        onInitializeReadCache();
        ImGui::LoadIniSettingsFromDisk(Config::Path::imGuiWindowsIni.generic_string().c_str());
    }

    void GameInstance::saveAll()
    {
        gameEditor.writeToCache();
        // gameScene.writeToCache();
        // world.writeToCache();
        ImGui::SaveIniSettingsToDisk(Config::Path::imGuiWindowsIni.generic_string().c_str());
        // writeToCache();

        onSaveAll();
    }

    /*void GameInstance::ioFieldsUpdate(DataStream& out)
    {
        auto stream = out.dedicatedNesting("GameInstance");

        int iRenderMode = static_cast<int>(renderMode);
        stream.field("renderMode", iRenderMode);
        renderMode = static_cast<RenderMode>(iRenderMode);
    }*/

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
            window->pollEvent();

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

        onLoadShaders();
    }

    StringAtom GameInstance::getCacheHash() const
    {
        return "GameInstance"_atom;
    }

    void GameInstance::loadCoreResources()
    {
        using namespace rectpack2D;

        constexpr const bool allow_flip = true;
        constexpr const int max_side = 1000;
        constexpr const int discard_step = -4;
        constexpr const auto runtime_flipping_mode = flipping_option::ENABLED;

        using spaces_type = empty_spaces<allow_flip>;
        using rect_type = output_rect_t<spaces_type>;

        std::vector<Texture> textures;
        std::vector<rect_type> rectangles;

        for (const auto& entry :
             std::filesystem::recursive_directory_iterator(Config::Path::images / "atlas"))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            const auto ext = entry.path().extension().string();

            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
            {
                Texture texture;
                if (texture.loadFromFile(entry.path()))
                {
                    rect_type rt;
                    rt.w = texture.getSize().width;
                    rt.h = texture.getSize().height;
                    rectangles.push_back(rt);

                    textures.push_back(std::move(texture));
                }
            }
        }

        auto report_successful = [](rect_type&) { return callback_result::CONTINUE_PACKING; };
        auto report_unsuccessful = [](rect_type&) { return callback_result::ABORT_PACKING; };

        auto report_result = [&rectangles](const rect_wh& result_size)
        {
            std::cout << "Resultant bin: " << result_size.w << " " << result_size.h << "\n";

            for (const auto& rect : rectangles)
            {
                const auto& r = rect.get_rect();
                std::cout << r.x << " " << r.y << " " << r.w << " " << r.h << "\n";
            }
        };

        const auto result_size = find_best_packing_dont_sort<spaces_type>(
            rectangles, make_finder_input(max_side, discard_step, report_successful,
                                          report_unsuccessful, runtime_flipping_mode));

        report_result(result_size);

        onLoadCoreResources();
    }
} // namespace Core