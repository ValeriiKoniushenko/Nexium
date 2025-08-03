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

#include "Core/Singleton.h"
#include "Editor/GameEditor.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Window.h"
#include "Misc/BaseLog.h"
#include "ModuleInfo.h"
#include "UserInterface.h"
#include "World.h"

namespace SW
{

    class GameInstance : public BaseLog
    {
    public:
        [[nodiscard]] spdlog::logger* getLogger() const override { return Framework::getLogger(); }
        [[nodiscard]] const char* getPrefix() const override { return "GameInstance"; }

        void init();

    public:
        GameEditor gameEditor;
        World world;
        UserInterface userInterface;

    protected:
        virtual void onTick(float delta) = 0;
        virtual void onInitFinish() = 0;
        virtual void onLoadShaders() = 0;

    protected:
        // Easy-access variables
        ShaderManager* shaderManager = nullptr;
        Window* window = nullptr;

        // Pre-launch settings
        std::filesystem::path assetsPath = "assets";
        std::filesystem::path shaderPath = "assets/shaders";
        Core::StringAtom spdlogDefaultPatter = "%D [%L] [%n] %v";
        Core::StringAtom defaultWindowName = "Sprite Walker";
        Core::ISize2 defaultWindowSize = Core::ISize2{ 1200, 800 };

    private:
        void gameLoop();
    };

} // namespace SW
