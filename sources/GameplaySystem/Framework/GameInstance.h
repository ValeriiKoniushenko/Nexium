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

#pragma once

#include "Camera/Camera.h"
#include "Editor/GameEditor.h"
#include "Graphics/RenderTargetToTexture.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Window.h"
#include "InputDevices/InputManager.h"
#include "Misc/BaseLog.h"
#include "ModuleInfo.h"
#include "Scene/Scene.h"
#include "Scene/Spectator.h"
#include "UserInterface.h"
#include "World.h"

namespace Core
{

    class GameInstance : public BaseLog
    {
    public:
        CreateEnum(RenderMode, int, GameOnly, Editor);

    public:
        [[nodiscard]] spdlog::logger* getLogger() const override { return Framework::getLogger(); }
        [[nodiscard]] const char* getPrefix() const override { return "GameInstance"; }

        void initialize();
        void updateViewport();
        void toggleRenderMode();

    public: // Temporary place. MOVE IT !!!!
        float windowAspectRatio = 16.f / 9.f;

    public:
        Scene gameScene;

        BaseCamera* currentCamera = nullptr;
        RenderMode renderMode = RenderMode::Editor;
        GameEditor gameEditor;
        RenderTargetToTexture renderToTextureObject;

        World world;
        UserInterface userInterface;

    protected:
        virtual void onTick(float delta) = 0;
        virtual void onInitFinish() = 0;
        virtual void onLoadShaders() = 0;
        virtual void onInitReadCache();
        virtual void onFinishWriteCache();

    protected:
        // Easy-access variables
        ShaderManager* _shaderManager = nullptr;
        Window* _window = nullptr;

        // Pre-launch settings TODO: MOVE IT!!!
        std::filesystem::path _assetsPath = "assets";
        std::filesystem::path _shaderPath = "assets/shaders";
        StringAtom _spdlogDefaultPatter = "%D [%L] [%n] %v";
        StringAtom _defaultWindowName = "Nexium";
        ISize2 _defaultWindowSize = ISize2{ 1200, 800 };

    private:
        void gameLoop();
    };

} // namespace Core

extern std::unique_ptr<Core::GameInstance> gameInstance;
