// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Scene-level drawing behaviour that the renderer does not deliver yet. These tests need a
// real GL context (Scene::directDraw lazily builds its font face and shaders), so they run on
// the GLContextTest fixture and are skipped on headless machines. Expected to FAIL until the
// documented behaviour exists; the comments say why it is the behaviour a game relies on.

#include "Foundation/Configs.h"
#include "Graphics/GLContextTest.h"
#include "NxSubsystems/Graphics/ShaderManager.h"
#include "NxSubsystems/Graphics/ShaderProgram.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Scene/Scene.h"
#include "NxWorld/Scene/SceneManager.h"
#include "Runtime/SceneFilesGuard.h"
#include "glm/gtc/matrix_transform.hpp"

#include <array>
#include <filesystem>
#include <memory>
#include <vector>

namespace
{
    constexpr int kFrame = 64;

    class DrawTestCamera : public NX::OrthographicCamera
    {
    public:
        const glm::mat4& getMatrix() override
        {
            _cachedProjMatrix = glm::ortho(0.f, static_cast<float>(kFrame), 0.f,
                                           static_cast<float>(kFrame), 0.1f, 10000.f);
            _cachedModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -1000.f));
            _cachedCalculatedMatrix = _cachedProjMatrix * _cachedModelMatrix;
            return _cachedCalculatedMatrix;
        }
    };

    class OrderRecordingDrawable : public NX::SceneObject
    {
    public:
        explicit OrderRecordingDrawable(std::vector<const SceneObject*>& log)
            : _log(log)
        {
        }

    protected:
        void onDraw(NX::BaseCamera&) override { _log.push_back(this); }

    private:
        std::vector<const SceneObject*>& _log;
    };

    class SceneDrawGapsTests : public GLContextTest
    {
    protected:
        void SetUp() override
        {
            GLContextTest::SetUp();
            if (IsSkipped())
            {
                return;
            }
            gGameInstance = std::make_unique<NX::GameInstance>(1, _arguments.data());
            NX::GetShaderManager().loadShaders(Foundation::Config::Path::shaders);
            // Same vertex layout GameInstance::initializeShaders installs for the text shader.
            if (auto* text = NX::GetShaderManager().getShaderProgram("text"_atom))
            {
                text->setSetEventCallback(
                    [](NX::ShaderProgram::Event event)
                    {
                        if ((event & NX::ShaderProgram::Event::OnSetVertexBuffer) != 0)
                        {
                            glEnableVertexAttribArray(0);
                            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                                                  nullptr);
                        }
                    });
            }
            _manager = std::make_unique<NX::SceneManager>(&gGameInstance->world);
            _scene = &_manager->createNewScene();
            // Like Spectator -> camera in real scenes: the camera is a child component.
            auto cameraHolder = NX::SceneObject::Create();
            cameraHolder->addChildComponent<DrawTestCamera>();
            _scene->addObjectToScene(std::move(cameraHolder));
            ASSERT_TRUE(_manager->setCurrentScene(_scene));
            ASSERT_NE(gGameInstance->world.currentCamera, nullptr);
            glViewport(0, 0, kFrame, kFrame);
        }

        void TearDown() override
        {
            // Free every GL object while the context still exists.
            _manager.reset();
            gGameInstance.reset();
            const auto emptyDir = std::filesystem::temp_directory_path() / "nexium-no-shaders";
            std::filesystem::create_directories(emptyDir);
            NX::GetShaderManager().loadShaders(emptyDir);
            GLContextTest::TearDown();
        }

        using Pixels = std::array<unsigned char, kFrame * kFrame * 4>;

        static void clear()
        {
            glClearColor(0.1f, 0.2f, 0.3f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }

        void draw()
        {
            clear();
            _scene->directDraw(nullptr, nullptr);
            glFinish();
        }

        [[nodiscard]] static Pixels readPixels()
        {
            Pixels pixels{};
            glReadBuffer(GL_BACK);
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glReadPixels(0, 0, kFrame, kFrame, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
            return pixels;
        }

        // Pixels that differ from what a bare clear produces (read back, so no rounding games).
        [[nodiscard]] static int countPixelsDifferentFromClearColour()
        {
            const Pixels drawn = readPixels();
            clear();
            glFinish();
            const Pixels cleared = readPixels();
            int different = 0;
            for (std::size_t i = 0; i < drawn.size(); i += 4)
            {
                different += (drawn[i] != cleared[i] || drawn[i + 1] != cleared[i + 1]
                              || drawn[i + 2] != cleared[i + 2])
                                 ? 1
                                 : 0;
            }
            return different;
        }

        std::array<char, 13> _executableName{ "Nexium_Tests" };
        std::array<char*, 1> _arguments{ _executableName.data() };
        SceneFilesGuard _sceneFiles;
        std::unique_ptr<NX::SceneManager> _manager;
        NX::Scene* _scene = nullptr;
    };
} // namespace

TEST_F(SceneDrawGapsTests, ASceneWithoutDrawableObjectsRendersNothing)
{
    // Scene::directDraw unconditionally draws the string "Hello world" at the world origin
    // (a text-feature leftover), in game builds as well as in the editor. A scene that
    // contains only a camera must leave the framebuffer at the clear colour; anything else is
    // content the game author never placed.
    draw();

    EXPECT_EQ(countPixelsDifferentFromClearColour(), 0);
}

TEST_F(SceneDrawGapsTests, DrawOrderFollowsDepthNotInsertionOrder)
{
    // The depth test is disabled for 2D (Runtime::runMainLoop), so what overlaps what is
    // purely the order Scene::directDraw visits objects: insertion order. A sprite's z (which
    // the gizmo keeps and the properties window exposes) has no effect on layering, and
    // re-layering means re-adding objects. For 2D/2.5D a painter's order by z (lower z drawn
    // first, higher z on top) is the behaviour content relies on.
    std::vector<const NX::SceneObject*> drawn;
    auto* front = new OrderRecordingDrawable(drawn);
    front->setPosition(NX::GPos3{ 0.f, 0.f, 5.f });
    auto* back = new OrderRecordingDrawable(drawn);
    back->setPosition(NX::GPos3{ 0.f, 0.f, 0.f });
    _scene->addObjectToScene(NX::SceneObject::Ptr(front));
    _scene->addObjectToScene(NX::SceneObject::Ptr(back));

    draw();

    ASSERT_EQ(drawn.size(), 2u);
    EXPECT_EQ(drawn[0], back);
    EXPECT_EQ(drawn[1], front);
}
