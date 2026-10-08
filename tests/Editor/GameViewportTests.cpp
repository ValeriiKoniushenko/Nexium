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
        using GameViewportEWC::updateImageArea;
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
        EXPECT_NO_THROW(viewport.deinitialize());
        EXPECT_NO_THROW(viewport.deinitialize());
    }

    TEST_F(GameViewportTests, ImageAreaDoesNotOverwriteWindowGeometryOrNotifyWindowResize)
    {
        TestGameViewport viewport;
        Core::DelegateSubscriberPoolGuard subscriptions;
        int windowResizeCount = 0;
        int imageResizeCount = 0;
        NX::FSize2 notifiedImageSize{};
        subscriptions << viewport.onSizeChanged->subscribeAndGetID([&](auto, auto)
                                                                   { ++windowResizeCount; });
        subscriptions << viewport.onImageSizeChanged->subscribeAndGetID(
            [&](auto size)
            {
                ++imageResizeCount;
                notifiedImageSize = size;
            });
        viewport.updateWindowArea();
        ASSERT_EQ(windowResizeCount, 1);
        const auto windowPosition = viewport.getInnerPosition();
        const auto windowSize = viewport.getInnerWindowSize();

        ImGui::Dummy(glm::vec2(200, 50));
        const auto imagePosition = ImGui::GetCursorScreenPos();
        const auto available = ImGui::GetContentRegionAvail();
        ASSERT_TRUE(viewport.updateImageArea());
        EXPECT_EQ(viewport.getInnerPosition(), windowPosition);
        EXPECT_EQ(viewport.getInnerWindowSize(), windowSize);
        EXPECT_EQ(windowResizeCount, 1);
        EXPECT_EQ(viewport.getImagePosition(), imagePosition);
        EXPECT_GT(viewport.getImagePosition().y, windowPosition.y);
        EXPECT_EQ(viewport.getImageSize(), NX::FSize2(available.x, available.y));
        EXPECT_EQ(notifiedImageSize, viewport.getImageSize());
        EXPECT_EQ(imageResizeCount, 1);

        ASSERT_TRUE(viewport.updateImageArea());
        EXPECT_EQ(imageResizeCount, 1);
        ImGui::Dummy(glm::vec2(200, 30));
        ASSERT_TRUE(viewport.updateImageArea());
        EXPECT_EQ(imageResizeCount, 2);
        EXPECT_EQ(windowResizeCount, 1);
        EXPECT_EQ(viewport.getInnerWindowSize(), windowSize);
    }

    TEST_F(GameViewportTests, EmptyImageAreaDoesNotRequestFramebufferResize)
    {
        TestGameViewport viewport;
        viewport.updateWindowArea();
        const auto windowPosition = viewport.getInnerPosition();
        const auto windowSize = viewport.getInnerWindowSize();
        Core::DelegateSubscriberPoolGuard subscriptions;
        int imageResizeCount = 0;
        subscriptions << viewport.onImageSizeChanged->subscribeAndGetID([&](auto)
                                                                        { ++imageResizeCount; });
        ImGui::Dummy(glm::vec2(200, 500));

        EXPECT_FALSE(viewport.updateImageArea());
        EXPECT_EQ(imageResizeCount, 0);
        EXPECT_EQ(viewport.getImageSize(), NX::FSize2{});
        EXPECT_EQ(viewport.getInnerPosition(), windowPosition);
        EXPECT_EQ(viewport.getInnerWindowSize(), windowSize);
    }
} // namespace
