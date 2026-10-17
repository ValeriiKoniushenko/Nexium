// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Runtime.h"

#include "Core/Size.h"
#include "Core/Timer.h"
#include "Foundation/Debug/Latency.h"
#include "Foundation/Debug/LatencyTerminalPrinter.h"
#include "NxRuntime/GameUtils/FPSCounter.h"
#include "NxSubsystems/Graphics/Font.h"
#include "NxSubsystems/Graphics/GraphicsComponents.h"
#include "NxSubsystems/Graphics/ShaderManager.h"
#include "NxSubsystems/Graphics/ShaderProgram.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Framework/InputSystem.h"
#include "NxWorld/Scene/Rectangle.h"
#include "NxWorld/Scene/SceneObject.h"
#include "Platform/Glfw.h"
#include "Platform/Graphics.h"
#include "Platform/Keyboard.h"
#include "RawBackend/Fonts/FreeTypeLibrary.h"
#include "RawBackend/Graphics/Texture.h"
#include "freetype/freetype.h"

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

#if defined(NEXIUM_DEBUG)
        const Foundation::Latency::Report r(Foundation::Latency::gCollector);
        Foundation::Latency::TerminalPrinter{ r }.print();
#endif
    }

    GameInstance& Runtime::getGameInstance() const noexcept
    {
        return _gameInstance;
    }

    void Runtime::runMainLoop()
    {
        NX_LATENCY_POINT("start");
        NX::FPSCounter fps;
        fps.start();
        Core::FStopwatch clock;

        GLint prev{};
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &prev);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        auto* shader = NX::GetShaderManager().getShaderProgram("text"_atom);

        NX::Face nxFace;
        nxFace.setSourcePath("data/internal/fonts/Inter/Inter_18pt-Black.ttf");
        nxFace.generateFace(0);
        nxFace.setPixelSize(48);
        nxFace.generateTextureASCIIGlyphs(shader);

        RawBackend::Font::Face face;
        face.setSourcePath("data/internal/fonts/Inter/Inter_18pt-Black.ttf");
        face.generateFace(0);
        face.setPixelSize(48);

        struct Character
        {
            UISize2 size;            // Size of glyph
            ISize2 bearing;          // Offset from baseline to left/top of glyph
            signed long advance = 0; // Offset to advance to next glyph
        };

        std::vector<Character> characters;
        std::vector<RawBackend::Texture> charText;
        characters.resize(128);
        charText.resize(128);

        std::string skippedGlyphs;
        for (unsigned char c = 0; c < 128; c++)
        {
            FT_GlyphSlot glyph = face.loadChar(c, FT_LOAD_RENDER);
            if (!glyph)
            {
                skippedGlyphs.push_back(c);
                continue;
            }

            characters[c] = Character{
                .size = UISize2{ glyph->bitmap.width, glyph->bitmap.rows },
                .bearing = ISize2{ glyph->bitmap_left, glyph->bitmap_top },
                .advance = glyph->advance.x,
            };

            auto&& rawChar = characters[c];
            auto& rawText = charText[c];

            rawText.generate();
            rawText.bind();
            // clang-format off
            rawText.putImage(
                0,
                GL_RED,
                static_cast<GLsizei>(rawChar.size.width),
                static_cast<GLsizei>(rawChar.size.height),
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                glyph->bitmap.buffer);
            // clang-format on
            rawText.generateMipmap(GL_LINEAR, GL_LINEAR);
        }

        face.release();
        glPixelStorei(GL_UNPACK_ALIGNMENT, prev);

        BaseGraphicsData g;
        g.generate();
        g.setShader(shader);
        shader->callSetEvent(ShaderProgram::Event::OnSetVertexBuffer);

        g.reserveVertexBuffer(sizeof(float) * 6 * 4, GL_DYNAMIC_DRAW);

        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_STENCIL_TEST);

        constexpr int clearBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;

        OrthographicCamera uiCamera;
        uiCamera.setNear(0.f);

        NX_LATENCY_POINT("while loop - pre-start");
        while (!_gameInstance.window->shouldClose())
        {
            NX_LATENCY_POINT("loop - start");
            clock.start();
            Platform::Window::pollEvent();
            GetInputSystem().setActiveContext(_gameInstance.isApplicationViewportFocused()
                                                  ? InputContext::Gameplay
                                                  : InputContext::Editor);
            GetInputSystem().processEvents();

            if (!_gameInstance.isEditorMode())
            {
                _gameInstance.gameScene.tick(_gameInstance.world.getTimeDelta());

                glClear(clearBits);

                if (auto* integration = _gameInstance.getApplicationIntegration())
                {
                    integration->updateInput();
                }

                auto ss = g.getShader();
                ss->use();
                uiCamera.invalidateCameraMatrices();
                ss->setUniform("uProj"_atom, uiCamera.getMatrix());

                glActiveTexture(GL_TEXTURE0);
                const std::string text = "Hello world";
                float xOffset = 0.f, yOffset = 0.f;

                for (char c : text)
                {
                    auto ch = characters[c];
                    float xpos = xOffset + static_cast<float>(ch.bearing.width);
                    float ypos = yOffset - (static_cast<float>(ch.size.height - ch.bearing.height));

                    float w = static_cast<float>(ch.size.width);
                    float h = static_cast<float>(ch.size.height);
                    // clang-format off
                    std::vector<float> vertices = {
                        0.f, h,    0.f, 0.f,
                        0.f, 0.f,  0.f, 1.f,
                        w,   0.f,  1.f, 1.f,
            
                        0.f, h,   0.f, 0.f,
                        w,   0.f,  1.f, 1.f,
                        w,   h,   1.f, 0.f,
                    };
                    // clang-format on
                    charText[c].bind();
                    g.setVertexSubBuffer(0, vertices);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                    xOffset += (ch.advance >> 6);
                }
                glBindVertexArray(0);
                glBindTexture(GL_TEXTURE_2D, 0);

                if (_gameInstance.world.currentCamera)
                {
                    _gameInstance.gameScene.directDraw(
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
                    _gameInstance.gameScene.tick(_gameInstance.world.getTimeDelta());
                }

                glClear(clearBits);
                integration->tick(_gameInstance.world.getTimeDelta());

                if (_gameInstance.world.currentCamera)
                {
                    integration->updateSceneInteraction(_gameInstance.gameScene);
                    integration->beforeSceneDraw();
                    glClear(clearBits);

                    _gameInstance.gameScene.directDraw(
                        _gameInstance.shaderManager.getShaderProgram("skybox"_atom),
                        _gameInstance.shaderManager.getShaderProgram("grid"_atom));
                    _gameInstance.tick(_gameInstance.world.getTimeDelta());
                    integration->afterSceneDraw();
                }
            }

            if (_gameInstance.window->isIconified() || !_gameInstance.window->isFocused())
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
            NX_LATENCY_POINT("loop - end");
        }
        NX_LATENCY_POINT("while loop - post-end");

        _gameInstance.infoLog("Total FPS for this session: {}"_f << fps.getFPS());
    }
} // namespace NX
