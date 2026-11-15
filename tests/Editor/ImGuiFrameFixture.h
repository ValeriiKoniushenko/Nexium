// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Editor/GuiComponents/Widget.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include "gtest/gtest.h"

namespace EditorTestUtils
{
    /// A headless ImGui context (no GL) with one open window per frame.
    class ImGuiFrameTest : public ::testing::Test
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
            ImGui::GetStyle().ItemSpacing = glm::vec2(8, 6);
            beginFrame();
        }

        void TearDown() override
        {
            endFrame();
            ImGui::DestroyContext();
        }

        static void beginFrame()
        {
            ImGui::NewFrame();
            ImGui::SetNextWindowPos(glm::vec2(0));
            ImGui::SetNextWindowSize(glm::vec2(800, 600));
            ImGui::Begin("EditorWidgetTests", nullptr, ImGuiWindowFlags_NoSavedSettings);
        }

        static void endFrame()
        {
            ImGui::End();
            ImGui::EndFrame();
        }

        static void nextFrame()
        {
            endFrame();
            beginFrame();
        }

        /// Makes the next item check meaningful: an ImGui item id of 0 means "nothing submitted".
        static void resetLastItem() { ImGui::Dummy(glm::vec2(0)); }

        static ImGuiID lastItemId() { return ImGui::GetItemID(); }
    };

    /// Records the ImGui id an unlabelled item would get and how often it was drawn.
    class Probe : public NX::Gui::Widget
    {
    public:
        Probe()
            : Widget("Probe"_atom)
        {
        }

        explicit Probe(std::size_t tag)
            : Widget("Probe"_atom),
              tag(tag)
        {
        }

        [[nodiscard]] float getWidth() const override { return 10.f; }
        [[nodiscard]] float getHeight() const override { return 10.f; }
        void setWidth(float /*newWidth*/) override {}
        void setHeight(float /*newHeight*/) override {}

        std::size_t tag = 0;
        ImGuiID lastId = 0;
        int draws = 0;

    protected:
        void onDraw() override
        {
            lastId = ImGui::GetID("");
            ++draws;
            ImGui::Dummy(glm::vec2(10.f));
        }
    };
} // namespace EditorTestUtils
