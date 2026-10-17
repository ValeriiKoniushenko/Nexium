// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/GuiComponents/Combo.h"

#include "gtest/gtest.h"

namespace
{
    template<class Combo>
    class PopupHookCombo : public Combo
    {
    public:
        void draw() { this->onDraw(); }
        bool popupWasDrawn = false;

    protected:
        void drawPopupContent() override { popupWasDrawn = true; }
    };

    class ComboPopupTests : public ::testing::Test
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
            ImGui::Begin("ComboPopupTests", nullptr, ImGuiWindowFlags_NoSavedSettings);
            ImGui::PushID("Combo");
        }

        void TearDown() override
        {
            ImGui::PopID();
            ImGui::End();
            ImGui::EndFrame();
            ImGui::DestroyContext();
        }
    };

    TEST_F(ComboPopupTests, ViewUsesPopupHook)
    {
        PopupHookCombo<NX::Gui::ComboView> combo;
        combo.setData({ "Scene"_atom });
        ImGui::OpenPopup("##ComboPopup");
        combo.draw();
        EXPECT_TRUE(combo.popupWasDrawn);
    }

    TEST_F(ComboPopupTests, ModelUsesPopupHook)
    {
        PopupHookCombo<NX::Gui::ComboModelBased> combo;
        combo.setSizeProvider([] { return 1; });
        combo.setDataProvider(
            [](std::size_t, Core::StringAtom& label) -> const void*
            {
                label = "Scene"_atom;
                return nullptr;
            });
        ImGui::OpenPopup("##ComboPopup");
        combo.draw();
        EXPECT_TRUE(combo.popupWasDrawn);
    }
} // namespace
