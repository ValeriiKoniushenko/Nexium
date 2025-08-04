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
#include "Graphics/Image.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Window.h"
#include "Misc/FPSCounter.h"

std::unique_ptr<SW::GameInstance> gameInstance = nullptr;

namespace SW
{

    void GameInstance::init()
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
            [this](Core::ISize2 newSize)
            {
                updateViewport();
            });
        //-------------------- SHADER MANAGER ---------------------
        _shaderManager = &GetShaderManager();
        _shaderManager->loadShaders(_shaderPath);
        _shaderManager->debugLog("Was loaded {} shaders."_f << _shaderManager->countOfShaders());
        for (const auto& notLoadedShader : _shaderManager->getFailedShaders())
        {
            _shaderManager->warnLog("Shader '{}' found but not loaded. It contains some error[s]."_f
                                    << notLoadedShader);
        }
        onLoadShaders();

        //-------------------- MISC ---------------------
        renderToTextureObject.generate();
        gameEditor.initialize();
        initShortcuts();
        onInitFinish();

        gameLoop();
    }

    void GameInstance::gameLoop()
    {
        Core::FStopwatch clock;

        FPSCounter fpsCounter;
        fpsCounter.start();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        while (!_window->shouldClose())
        {
            clock.start();
            _window->pollEvent();

            keyboardInput.update();
            mouseInput.update();

            if (renderMode.cast() == RenderMode::GameOnly)
            {
                glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                onTick(world.timeDelta);
            }
            else
            {
                renderToTextureObject.callMePreDraw();
                glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                onTick(world.timeDelta);
                renderToTextureObject.callMeAfterDraw();

                glClearColor(0.45f, 0.65f, 0.40f, 1.00f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                gameEditor.onTick(world.timeDelta);
            }

            _window->swapBuffers();
            fpsCounter.newFrameUpdate();
            world.timeDelta = clock.stop();
        }

        globalLog.infoLog("FPS: {}"_f << fpsCounter.getFPS());
    }
    void GameInstance::initShortcuts()
    {
        static auto getRealSpeed = [this](SW::KeyboardIA::SpecKeysState state)
        {
            const float mlt
                = state.leftShift.cast() == SW::Keyboard::KeyState::Pressed ? 10.f : 1.f;
            return speed * mlt;
        };
        auto toggleSimulation = keyboardInput.create("toggleSimulation", GLFW_KEY_F12);
        toggleSimulation->setIsRepeatable(false);
        toggleSimulation->onPress.subscribe(
            [this](auto)
            {
                renderMode = renderMode.cast() == RenderMode::GameOnly ? RenderMode::Editor
                                                                       : RenderMode::GameOnly;
                updateViewport();
            });

        // clang-format off
        keyboardInput.create("moveForward", GLFW_KEY_W)->onPress.subscribe([&](auto state){ camera.moveForward(getRealSpeed(state) * world.timeDelta); });
        keyboardInput.create("moveBackward", GLFW_KEY_S)->onPress.subscribe([&](auto state){ camera.moveForward(-getRealSpeed(state) * world.timeDelta); });
        keyboardInput.create("moveRight", GLFW_KEY_D)->onPress.subscribe([&](auto state){ camera.moveRight(-getRealSpeed(state) * world.timeDelta); });
        keyboardInput.create("moveLeft", GLFW_KEY_A)->onPress.subscribe([&](auto state){ camera.moveRight(getRealSpeed(state) *   world.timeDelta); });
        keyboardInput.create("moveUp", GLFW_KEY_SPACE)->onPress.subscribe([&](auto state){ camera.moveUp(-getRealSpeed(state) *  world.timeDelta); });
        keyboardInput.create("moveDown", GLFW_KEY_C)->onPress.subscribe([&](auto state){ camera.moveUp(getRealSpeed(state) *     world.timeDelta); });
        keyboardInput.create("exit", GLFW_KEY_ESCAPE)->onPress.subscribe([&](auto){ _window->close(); });
        const auto toggleCursorMode = keyboardInput.create("toggleCursorMode", GLFW_KEY_M);
        toggleCursorMode->onPress.subscribe([&](auto) { _window->toggleCursorMode(); });
        toggleCursorMode->setIsRepeatable(false);
        mouseInput.create("cameraView", 0)->onMove.subscribe([&](glm::vec2 delta, auto){ camera.yawAndPitch(delta * world.timeDelta * mouseSensitivity); });
        // clang-format on
    }

    void GameInstance::updateViewport()
    {
        if (renderMode.cast() == RenderMode::GameOnly)
        {
            _window->updateViewport(windowAspectRatio, ViewportMode::ZoomIn);
        }
        else
        {
            UpdateGlViewport(static_cast<Core::FSize2>(renderToTextureObject.getRenderSize()),
                             windowAspectRatio, ViewportMode::ZoomIn);
        }
    }
} // namespace SW