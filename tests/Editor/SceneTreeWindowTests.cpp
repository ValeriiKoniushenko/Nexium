// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/Windows/Scenes/SceneTreeWindow.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "NxWorld/Framework/GameInstance.h"
#include "Runtime/SceneFilesGuard.h"

#include "gtest/gtest.h"
#include <array>
#include <functional>

namespace
{
    TEST(SceneTreeWindow, HasDefaultNameBeforeInitialization)
    {
        const NX::SceneTreeWindowEWC window;
        EXPECT_FALSE(window.isInitialized());
        EXPECT_EQ(window.getComponentName(), "Scene"_atom);
    }

    TEST(SceneTreeWindow, PreservesExplicitNameDuringConstruction)
    {
        const NX::SceneTreeWindowEWC window("Custom scene tree"_atom);
        EXPECT_EQ(window.getComponentName(), "Custom scene tree"_atom);
    }

    class TestSceneTreeWindow : public NX::SceneTreeWindowEWC
    {
    public:
        using SceneTreeWindowEWC::onDraw;
    };

    class SceneTreeRenameTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            auto executable = std::to_array("Nexium_Tests");
            std::array<char*, 1> arguments{ executable.data() };
            gGameInstance = std::make_unique<NX::GameInstance>(1, arguments.data());
            ImGui::CreateContext();
            auto& io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.DisplaySize = glm::vec2(800, 600);
            unsigned char* pixels = nullptr;
            int width = 0;
            int height = 0;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        }

        void TearDown() override
        {
            ImGui::DestroyContext();
            gGameInstance.reset();
        }

        // Draws the tree inside a test-owned window; `inWindow` runs after the tree.
        void drawFrame(const std::function<void()>& inWindow = {})
        {
            ImGui::NewFrame();
            ImGui::SetNextWindowPos(glm::vec2(10, 10));
            ImGui::SetNextWindowSize(glm::vec2(400, 300));
            ImGui::Begin("SceneTreeRenameTests", nullptr, ImGuiWindowFlags_NoSavedSettings);
            tree.onDraw();
            if (inWindow)
            {
                inWindow();
            }
            ImGui::End();
            ImGui::EndFrame();
        }

        TestSceneTreeWindow tree;
        SceneFilesGuard sceneFiles;
    };

    TEST_F(SceneTreeRenameTests, RenamesSceneOnlyWhenEditingFinishes)
    {
        auto* scene = gGameInstance->scenes.getCurrentScene();
        const auto oldName = scene->getSceneName();
        tree.setScene(scene);
        drawFrame([] { ImGui::ActivateItemByID(ImGui::GetID("##sceneName")); });
        drawFrame();

        auto& io = ImGui::GetIO();
        io.AddKeyEvent(ImGuiMod_Ctrl, true);
        io.AddKeyEvent(ImGuiKey_A, true);
        io.AddKeyEvent(ImGuiKey_A, false);
        io.AddKeyEvent(ImGuiMod_Ctrl, false);
        drawFrame();
        for (const char* text : { "Re", "named", "Tree" })
        {
            io.AddInputCharactersUTF8(text);
            drawFrame();
            EXPECT_EQ(scene->getSceneName(), oldName);
        }

        io.AddKeyEvent(ImGuiKey_Enter, true);
        io.AddKeyEvent(ImGuiKey_Enter, false);
        drawFrame();
        EXPECT_EQ(scene->getSceneName(), "RenamedTree"_atom);
    }

    TEST_F(SceneTreeRenameTests, RefusedRenameRestoresTheSceneName)
    {
        auto& scenes = gGameInstance->scenes;
        auto* scene = scenes.getCurrentScene();
        const auto oldName = scene->getSceneName();
        const auto otherName = scenes.createNewScene().getSceneName();
        tree.setScene(scene);
        drawFrame([] { ImGui::ActivateItemByID(ImGui::GetID("##sceneName")); });
        drawFrame();

        auto& io = ImGui::GetIO();
        io.AddKeyEvent(ImGuiMod_Ctrl, true);
        io.AddKeyEvent(ImGuiKey_A, true);
        io.AddKeyEvent(ImGuiKey_A, false);
        io.AddKeyEvent(ImGuiMod_Ctrl, false);
        io.AddInputCharactersUTF8(otherName.c_str());
        drawFrame();
        io.AddKeyEvent(ImGuiKey_Enter, true);
        io.AddKeyEvent(ImGuiKey_Enter, false);
        drawFrame();
        EXPECT_EQ(scene->getSceneName(), oldName);
    }
} // namespace
