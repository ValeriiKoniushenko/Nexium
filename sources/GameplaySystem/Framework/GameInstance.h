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

#pragma once

#include "AssetsManager/AssetsManager.h"
#include "Camera/Camera.h"
#include "Editor/GameEditor.h"
#include "Editor/ObjectSelectorManager.h"
#include "Graphics/RenderTargetToTexture.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Window.h"
#include "ModuleInfo.h"
#include "Scene/Scene.h"
#include "UserInterface.h"
#include "World.h"

namespace Core
{
    class GameInstance : public BaseLog, public IDataStreamBridge
    {
    public:
        enum class RenderMode
        {
            GameOnly,
            Editor
        };

    public:
        [[nodiscard]] spdlog::logger* getLogger() const override { return Framework::getLogger(); }
        [[nodiscard]] const char* getPrefix() const override { return "GameInstance"; }

        void initialize();

        void updateViewport();

        void toggleRenderMode();

        void saveAll();

        void ioFieldsUpdate(DataStream& out) override;

    public:
        Scene gameScene;
        ShaderManager shaderManager;
        World world;
        UserInterface userInterface;
        AssetsManager assets;

        BaseCamera* currentCamera = nullptr;
        RenderMode renderMode = RenderMode::Editor;
        GameEditor gameEditor;
        RenderTargetToTexture gameViewport;
        ObjectSelectorManager objectSelectorManager;
        Window* window = nullptr;

    protected:
        virtual void onSaveAll() {}
        virtual void onTick(float delta) {}
        virtual void onLoadShaders() {}
        virtual void onLoadCoreResources() {}
        virtual void onInitializeReadCache() {}

        [[nodiscard]] StringAtom getCacheHash() const override;

    private:
        void loadCoreResources();
        void startUpReadCache();
        void initializeShaders();
        void runMainLoop();
    };

    [[nodiscard]] ShaderManager& GetShaderManager();

    [[nodiscard]] GameEditor& GetEditor();

    [[nodiscard]] World& GetWorld();

    [[nodiscard]] AssetsManager& GetAssetsManager();
} // namespace Core

extern std::unique_ptr<Core::GameInstance> gGameInstance;