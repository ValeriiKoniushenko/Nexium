// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/Windows/GameViewport.h"
#include "ImGui/imgui.h"

#include "gtest/gtest.h"

namespace
{
    class TestGameViewport : public NX::GameViewportEWC
    {
    public:
        void updateWindowArea() { BaseFloatEWC::onUpdate(); }
        using GameViewportEWC::updateCanvasArea;
    };

    class GameViewportTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            ImGui::CreateContext();
            auto& io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.DisplaySize = glm::vec2(800, 600);
            unsigned char* pixels = nullptr;
            int width = 0;
            int height = 0;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
            ImGui::NewFrame();
            ImGui::SetNextWindowPos(glm::vec2(30, 40));
            ImGui::SetNextWindowSize(glm::vec2(600, 400));
            ImGui::Begin("GameViewportTests", nullptr, ImGuiWindowFlags_NoSavedSettings);
        }

        void TearDown() override
        {
            ImGui::End();
            ImGui::EndFrame();
            ImGui::DestroyContext();
        }
    };

    TEST_F(GameViewportTests, DeinitializationBeforeInitializationIsSafe)
    {
        TestGameViewport viewport;
        viewport.deinitialize();
        viewport.deinitialize();
        EXPECT_FALSE(viewport.isInitialized());
    }

    TEST_F(GameViewportTests, CanvasAreaDoesNotOverwriteWindowGeometryOrNotifyWindowResize)
    {
        TestGameViewport viewport;
        Core::DelegateSubscriberPoolGuard subscriptions;
        int windowResizeCount = 0;
        int canvasResizeCount = 0;
        NX::FSize2 notifiedCanvasSize{};
        subscriptions << viewport.onSizeChanged->subscribeAndGetID([&](auto, auto)
                                                                   { ++windowResizeCount; });
        subscriptions << viewport.onCanvasSizeChanged->subscribeAndGetID(
            [&](auto size)
            {
                ++canvasResizeCount;
                notifiedCanvasSize = size;
            });
        viewport.updateWindowArea();
        ASSERT_EQ(windowResizeCount, 1);
        const auto windowPosition = viewport.getInnerPosition();
        const auto windowSize = viewport.getInnerWindowSize();

        ImGui::Dummy(glm::vec2(200, 50));
        const auto canvasPosition = ImGui::GetCursorScreenPos();
        const auto available = ImGui::GetContentRegionAvail();
        ASSERT_TRUE(viewport.updateCanvasArea());
        EXPECT_EQ(viewport.getInnerPosition(), windowPosition);
        EXPECT_EQ(viewport.getInnerWindowSize(), windowSize);
        EXPECT_EQ(windowResizeCount, 1);
        EXPECT_EQ(viewport.getCanvasPosition(), canvasPosition);
        EXPECT_GT(viewport.getCanvasPosition().y, windowPosition.y);
        EXPECT_EQ(viewport.getCanvasSize(), NX::FSize2(available.x, available.y));
        EXPECT_EQ(notifiedCanvasSize, viewport.getCanvasSize());
        EXPECT_EQ(canvasResizeCount, 1);

        ASSERT_TRUE(viewport.updateCanvasArea());
        EXPECT_EQ(canvasResizeCount, 1);
        ImGui::Dummy(glm::vec2(200, 30));
        ASSERT_TRUE(viewport.updateCanvasArea());
        EXPECT_EQ(canvasResizeCount, 2);
        EXPECT_EQ(windowResizeCount, 1);
        EXPECT_EQ(viewport.getInnerWindowSize(), windowSize);
    }

    TEST_F(GameViewportTests, EmptyCanvasAreaDoesNotRequestFramebufferResize)
    {
        TestGameViewport viewport;
        viewport.updateWindowArea();
        const auto windowPosition = viewport.getInnerPosition();
        const auto windowSize = viewport.getInnerWindowSize();
        Core::DelegateSubscriberPoolGuard subscriptions;
        int canvasResizeCount = 0;
        subscriptions << viewport.onCanvasSizeChanged->subscribeAndGetID([&](auto)
                                                                         { ++canvasResizeCount; });
        ImGui::Dummy(glm::vec2(200, 500));

        EXPECT_FALSE(viewport.updateCanvasArea());
        EXPECT_EQ(canvasResizeCount, 0);
        EXPECT_EQ(viewport.getCanvasSize(), NX::FSize2{});
        EXPECT_EQ(viewport.getInnerPosition(), windowPosition);
        EXPECT_EQ(viewport.getInnerWindowSize(), windowSize);
    }

    TEST_F(GameViewportTests, CanvasShapeChangeNotifiesEvenWhenAreaIsUnchanged)
    {
        TestGameViewport viewport;
        Core::DelegateSubscriberPoolGuard subscriptions;
        int resizeCount = 0;
        subscriptions << viewport.onCanvasSizeChanged->subscribeAndGetID([&](auto)
                                                                         { ++resizeCount; });
        const auto regionEnd = ImGui::GetCursorScreenPos() + ImGui::GetContentRegionAvail();

        ImGui::SetCursorScreenPos(regionEnd - glm::vec2(200.f, 100.f));
        ASSERT_TRUE(viewport.updateCanvasArea());
        EXPECT_EQ(resizeCount, 1);

        ImGui::SetCursorScreenPos(regionEnd - glm::vec2(100.f, 200.f));
        ASSERT_TRUE(viewport.updateCanvasArea());
        EXPECT_EQ(resizeCount, 2);
        EXPECT_FLOAT_EQ(viewport.getCanvasSize().width, 100.f);
        EXPECT_FLOAT_EQ(viewport.getCanvasSize().height, 200.f);
        ImGui::Dummy(glm::vec2{});
    }
} // namespace
