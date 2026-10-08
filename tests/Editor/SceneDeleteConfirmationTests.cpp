// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/Windows/Scenes/SceneTabs/SceneDeleteConfirmation.h"
#include "NxWorld/Framework/GameInstance.h"

#include "gtest/gtest.h"

namespace
{
    static_assert(std::derived_from<NX::SceneDeleteConfirmation, NX::BaseModalPopUp>);

    class SceneDeleteConfirmationTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            char executable[] = "Nexium_Tests";
            char* arguments[] = { executable };
            gGameInstance = std::make_unique<NX::GameInstance>(1, arguments);
            ImGui::CreateContext();
            auto& io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.DisplaySize = glm::vec2(800, 600);
            unsigned char* pixels = nullptr;
            int width = 0;
            int height = 0;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
            ImGui::NewFrame();
        }

        void TearDown() override
        {
            ImGui::EndFrame();
            ImGui::DestroyContext();
            gGameInstance.reset();
        }

        static void nextFrame()
        {
            ImGui::EndFrame();
            ImGui::NewFrame();
        }
    };

    TEST_F(SceneDeleteConfirmationTests, UsesWindowLifecycleAndReopensAfterClose)
    {
        auto popup = NX::SceneDeleteConfirmation::Create();
        popup->closeWindow();
        popup->open(nullptr);
        EXPECT_FALSE(popup->isEnabled());
        auto& scene = gGameInstance->scenes.createNewScene();
        popup->open(&scene);
        EXPECT_TRUE(popup->isInitialized());
        EXPECT_TRUE(popup->isEnabled());
        popup->tick(0.f);
        EXPECT_TRUE(ImGui::IsPopupOpen("Delete scene?"));
        EXPECT_EQ(popup->getWindowSize(), NX::FSize2(420.f, 180.f));

        popup->closeWindow();
        EXPECT_FALSE(popup->isEnabled());
        nextFrame();
        popup->open(&scene);
        popup->tick(0.f);
        EXPECT_TRUE(popup->isEnabled());
        EXPECT_TRUE(ImGui::IsPopupOpen("Delete scene?"));
    }

    TEST_F(SceneDeleteConfirmationTests, ClosesWhenSceneIsRemovedEvenIfRetainedElsewhere)
    {
        auto& scenes = gGameInstance->scenes;
        Core::IntrusivePtr<NX::Scene> scene = &scenes.createNewScene();
        NX::SceneDeleteConfirmation popup;
        popup.open(scene.get());
        popup.tick(0.f);
        ASSERT_TRUE(popup.isEnabled());
        scenes.removeScene(scene.get());
        nextFrame();
        popup.tick(0.f);
        EXPECT_FALSE(popup.isEnabled());
        EXPECT_FALSE(ImGui::IsPopupOpen("Delete scene?"));
    }
} // namespace
