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

namespace SW
{

    void GameInstance::init()
    {
#ifdef DEBUG
        spdlog::set_level(spdlog::level::trace);
#endif
        std::cout << std::fixed << std::setprecision(15);
        spdlog::set_pattern(spdlogDefaultPatter.toStdString());

        //    _    _  _             _
        //   | |  | |(_)           | |
        //   | |  | | _  _ __    __| |  ___ __      __
        //   | |/\| || || '_ \  / _` | / _ \\ \ /\ / /
        //   \  /\  /| || | | || (_| || (_) |\ V  V /
        //    \/  \/ |_||_| |_| \__,_| \___/  \_/\_/
        //----------------------------------------------
        window = &GetWindow();
        window->create(defaultWindowName, defaultWindowSize);

        //    _____  _                 _
        //   /  ___|| |               | |
        //   \ `--. | |__    __ _   __| |  ___  _ __  ___
        //    `--. \| '_ \  / _` | / _` | / _ \| '__|/ __|
        //   /\__/ /| | | || (_| || (_| ||  __/| |   \__ \
        //   \____/ |_| |_| \__,_| \__,_| \___||_|   |___/
        //-------------------------------------------------
        shaderManager = &GetShaderManager();
        shaderManager->loadShaders(shaderPath);
        shaderManager->debugLog("Was loaded {} shaders."_f << shaderManager->countOfShaders());
        for (const auto& notLoadedShader : shaderManager->getFailedShaders())
        {
            shaderManager->warnLog("Shader '{}' found but not loaded. It contains some error[s]."_f
                                   << notLoadedShader);
        }
        onLoadShaders();

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
        while (!window->shouldClose())
        {
            clock.start();
            window->pollEvent();
            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            onTick();

            window->swapBuffers();
            fpsCounter.newFrameUpdate();
            world.timeDelta = clock.stop();
        }

        globalLog.infoLog("FPS: {}"_f << fpsCounter.getFPS());
    }
} // namespace SW