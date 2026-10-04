// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/Windows/ECSAdapters/Input/ECSEditorInputControllerAdapter.h"
#include "Editor/Windows/Editors/InputBindings/BindingsListWidget.h"
#include "Editor/Windows/Editors/InputBindings/InputBindingsEditor.h"
#include "Editor/Windows/NxECSBasedEditor.h"
#include "ImGui/imgui_internal.h"
#include "NxWorld/Framework/GameInstance.h"

#include "gtest/gtest.h"
#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace
{
    using namespace NX;

    class TestInputBindingsEditor : public InputBindingsEditor
    {
    public:
        using InputBindingsEditor::onDraw;

        Core::StringAtom getCacheHash() const override { return "BindingsListWidgetTests"_atom; }
    };

    class BindingsTestOwner : public NxECSBasedEditorEWC
    {
    public:
        Core::StringAtom getCacheHash() const override { return "BindingsTestOwner"_atom; }

        void selectComponent(const BaseComponent::Ptr& component) { _targetComponent = component; }
    };

    class BindingsListWidgetTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            auto executable = std::to_array("Nexium_Tests");
            std::array<char*, 1> arguments{ executable.data() };
            _previousGameInstance = std::move(gGameInstance);
            gGameInstance = std::make_unique<GameInstance>(1, arguments.data());

            ImGui::CreateContext();
            auto& io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.DisplaySize = glm::vec2(1100, 700);
            unsigned char* pixels = nullptr;
            int width = 0;
            int height = 0;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        }

        void TearDown() override
        {
            ImGui::DestroyContext();
            gGameInstance = std::move(_previousGameInstance);
        }

        template<class Draw>
        static std::string captureDrawnText(const Draw& draw)
        {
            ImGui::NewFrame();
            ImGui::SetNextWindowPos(glm::vec2(0));
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
            ImGui::Begin("BindingsListWidgetTests", nullptr, ImGuiWindowFlags_NoSavedSettings);
            ImGui::LogToBuffer();
            draw();
            const std::string text = GImGui->LogBuffer.c_str();
            ImGui::LogFinish();
            ImGui::End();
            ImGui::EndFrame();
            return text;
        }

        // Locate rendered labels so mouse interaction tests do not depend on panel coordinates.
        static std::vector<glm::vec2> renderedTextPositions(std::string_view text)
        {
            auto* font = ImGui::GetIO().Fonts->Fonts[0];
            auto* bakedFont = font->GetFontBaked(font->LegacySize);
            std::vector<const ImFontGlyph*> glyphs;
            for (const auto character : text)
            {
                const auto* glyph = bakedFont->FindGlyph(
                    static_cast<ImWchar>(static_cast<unsigned char>(character)));
                if (glyph && glyph->Visible)
                {
                    glyphs.push_back(glyph);
                }
            }

            std::vector<glm::vec2> positions;
            if (glyphs.empty())
            {
                return positions;
            }

            for (const auto* window : GImGui->Windows)
            {
                if (!window->Active || window->Hidden)
                {
                    continue;
                }
                const auto& vertices = window->DrawList->VtxBuffer;
                for (int start = 0; start + static_cast<int>(glyphs.size() * 4) <= vertices.Size;
                     ++start)
                {
                    bool matches = true;
                    ImRect bounds(vertices[start].pos, vertices[start + 2].pos);
                    for (std::size_t i = 0; i < glyphs.size(); ++i)
                    {
                        const auto offset = start + static_cast<int>(i * 4);
                        const auto* glyph = glyphs[i];
                        if (vertices[offset].uv != glm::vec2(glyph->U0, glyph->V0)
                            || vertices[offset + 2].uv != glm::vec2(glyph->U1, glyph->V1))
                        {
                            matches = false;
                            break;
                        }
                        bounds.Add(vertices[offset].pos);
                        bounds.Add(vertices[offset + 2].pos);
                    }
                    if (matches)
                    {
                        positions.push_back(bounds.GetCenter());
                    }
                }
            }
            return positions;
        }

        template<class Draw>
        static void clickAt(const Draw& draw, glm::vec2 position)
        {
            auto& io = ImGui::GetIO();
            io.AddMousePosEvent(position.x, position.y);
            captureDrawnText(draw);
            io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
            captureDrawnText(draw);
            io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
            captureDrawnText(draw);
        }

        template<class Draw>
        static void clickText(const Draw& draw, std::string_view text)
        {
            captureDrawnText(draw);
            captureDrawnText(draw);
            const auto positions = renderedTextPositions(text);
            ASSERT_FALSE(positions.empty()) << "Missing rendered control: " << text;
            clickAt(draw, positions.front());
        }

        template<class Draw>
        static void selectAllText(const Draw& draw)
        {
            auto& io = ImGui::GetIO();
            io.AddKeyEvent(ImGuiMod_Ctrl, true);
            io.AddKeyEvent(ImGuiKey_A, true);
            captureDrawnText(draw);
            io.AddKeyEvent(ImGuiKey_A, false);
            io.AddKeyEvent(ImGuiMod_Ctrl, false);
            captureDrawnText(draw);
        }

    private:
        std::unique_ptr<GameInstance> _previousGameInstance;
    };

    TEST_F(BindingsListWidgetTests, DisplaysOwnedBindingsPassedBeforeInitialization)
    {
        BindingsListWidget widget;
        std::vector<Core::StringAtom> bindings{ "Jump"_atom, "Move left"_atom };
        widget.setBindings(bindings);
        bindings.clear();
        widget.initialize();

        const auto text = captureDrawnText([&widget] { widget.draw(); });

        EXPECT_NE(text.find("Jump"), std::string::npos);
        EXPECT_NE(text.find("Move left"), std::string::npos);
    }

    TEST_F(BindingsListWidgetTests, ReplacesAndClearsDisplayedBindingsAfterInitialization)
    {
        BindingsListWidget widget;
        widget.initialize();
        widget.setBindings({ "Old action"_atom });
        EXPECT_NE(captureDrawnText([&widget] { widget.draw(); }).find("Old action"),
                  std::string::npos);

        widget.setBindings({ "Jump"_atom, "Attack"_atom });
        const auto replaced = captureDrawnText([&widget] { widget.draw(); });
        EXPECT_EQ(replaced.find("Old action"), std::string::npos);
        EXPECT_NE(replaced.find("Jump"), std::string::npos);
        EXPECT_NE(replaced.find("Attack"), std::string::npos);

        widget.setBindings({});
        const auto cleared = captureDrawnText([&widget] { widget.draw(); });
        EXPECT_EQ(cleared.find("Jump"), std::string::npos);
        EXPECT_EQ(cleared.find("Attack"), std::string::npos);
    }

    TEST_F(BindingsListWidgetTests, EditorDisplaysSettingsForTheSelectedFullBinding)
    {
        using Key = Platform::Keyboard::Key;
        TestInputBindingsEditor editor;
        editor.setBindings(
            { { .action = "Jump"_atom,
                .chord = KeyChord::Exact(Key::Space),
                .trigger = InputActionTrigger::OnPress },
              { .action = "Save"_atom,
                .chord = { .triggerKey = Key::S, .requiredKeys = { Key::Left_Control } },
                .trigger = InputActionTrigger::OnRelease } });
        editor.initialize();
        editor.selectBinding(0);
        const auto first = captureDrawnText([&editor] { editor.onDraw(); });
        EXPECT_NE(first.find("Shortcut settings"), std::string::npos);
        EXPECT_NE(first.find("Space"), std::string::npos);
        EXPECT_NE(first.find("On press"), std::string::npos);

        editor.selectBinding(1);
        const auto second = captureDrawnText([&editor] { editor.onDraw(); });
        EXPECT_EQ(second.find("Space"), std::string::npos);
        EXPECT_NE(second.find("Ctrl + S"), std::string::npos);
        EXPECT_NE(second.find("On release"), std::string::npos);
    }

    TEST_F(BindingsListWidgetTests, ClickingDuplicateActionSelectsTheCorrectBinding)
    {
        using Key = Platform::Keyboard::Key;
        TestInputBindingsEditor editor;
        editor.setBindings({ { .action = "Jump"_atom, .chord = KeyChord::Exact(Key::Space) },
                             { .action = "Jump"_atom,
                               .chord = KeyChord::Exact(Key::Enter),
                               .trigger = InputActionTrigger::OnRelease } });
        editor.initialize();
        editor.selectBinding(0);
        const auto draw = [&editor] { editor.onDraw(); };
        captureDrawnText(draw);
        captureDrawnText(draw);
        auto positions = renderedTextPositions("Jump");
        std::ranges::sort(positions, [](glm::vec2 left, glm::vec2 right)
                          { return left.x == right.x ? left.y < right.y : left.x < right.x; });
        ASSERT_GE(positions.size(), 2);
        ASSERT_FLOAT_EQ(positions[0].x, positions[1].x);
        clickAt(draw, positions[1]);

        const auto selected = captureDrawnText(draw);
        EXPECT_NE(selected.find("Enter"), std::string::npos);
        EXPECT_NE(selected.find("On release"), std::string::npos);
        EXPECT_EQ(selected.find("Space"), std::string::npos);
    }

    TEST_F(BindingsListWidgetTests, ActionAndTriggerEditsPersistToTheSelectedControllerBinding)
    {
        using Key = Platform::Keyboard::Key;
        InputController::Ptr controller = new InputController;
        controller->setBindings({ { .action = "Jump"_atom, .chord = KeyChord::Exact(Key::Space) },
                                  { .action = "Attack"_atom, .chord = KeyChord::Exact(Key::A) } });
        Core::IntrusivePtr<BindingsTestOwner> owner = new BindingsTestOwner;
        owner->selectComponent(controller);
        TestInputBindingsEditor editor;
        editor.initialize();
        editor.setTarget(controller.get(), owner.get());
        editor.selectBinding(1);
        EXPECT_FALSE(owner->isDirty());
        const auto draw = [&editor] { editor.onDraw(); };
        captureDrawnText(draw);
        captureDrawnText(draw);
        auto actionPositions = renderedTextPositions("Attack");
        ASSERT_FALSE(actionPositions.empty());
        const auto actionPosition = *std::ranges::max_element(
            actionPositions, {}, [](glm::vec2 position) { return position.x; });
        clickAt(draw, actionPosition);

        auto& io = ImGui::GetIO();
        selectAllText(draw);
        io.AddInputCharactersUTF8("Strike");
        captureDrawnText(draw);

        ASSERT_EQ(controller->getBindings().size(), 2);
        EXPECT_EQ(controller->getBindings()[0].action, "Jump"_atom);
        EXPECT_EQ(controller->getBindings()[1].action, "Strike"_atom);
        EXPECT_TRUE(owner->isDirty());

        selectAllText(draw);
        io.AddKeyEvent(ImGuiKey_Backspace, true);
        captureDrawnText(draw);
        io.AddKeyEvent(ImGuiKey_Backspace, false);
        const auto unnamed = captureDrawnText(draw);
        EXPECT_TRUE(controller->getBindings()[1].action.isEmpty());
        EXPECT_NE(unnamed.find("Unnamed action"), std::string::npos);

        const std::string longAction(512, 'L');
        io.AddInputCharactersUTF8(longAction.c_str());
        captureDrawnText(draw);
        EXPECT_EQ(controller->getBindings()[1].action.toStdString(), longAction);

        clickText(draw, "On press");
        clickText(draw, "On release");
        EXPECT_EQ(controller->getBindings()[1].trigger, InputActionTrigger::OnRelease);
        EXPECT_EQ(controller->getBindings()[1].chord.triggerKey, Key::A);
        EXPECT_EQ(controller->getBindings()[0].trigger, InputActionTrigger::OnPress);
    }

    TEST_F(BindingsListWidgetTests, AddAndDeleteButtonsPersistRealBindings)
    {
        using Key = Platform::Keyboard::Key;
        InputController::Ptr controller = new InputController;
        controller->setBindings(
            { { .action = "Jump"_atom, .chord = KeyChord::Exact(Key::Space) } });
        Core::IntrusivePtr<BindingsTestOwner> owner = new BindingsTestOwner;
        owner->selectComponent(controller);
        TestInputBindingsEditor editor;
        editor.initialize();
        editor.setTarget(controller.get(), owner.get());
        const auto draw = [&editor] { editor.onDraw(); };

        clickText(draw, "Add binding");
        ASSERT_EQ(controller->getBindings().size(), 2);
        EXPECT_EQ(controller->getBindings()[1].chord.triggerKey, Key::None);
        EXPECT_EQ(controller->getBindings()[1].trigger, InputActionTrigger::OnPress);
        EXPECT_TRUE(owner->isDirty());

        clickText(draw, "Delete shortcut");
        ASSERT_EQ(controller->getBindings().size(), 1);
        EXPECT_EQ(controller->getBindings()[0].action, "Jump"_atom);
        EXPECT_EQ(controller->getBindings()[0].chord.triggerKey, Key::Space);

        clickText(draw, "Delete shortcut");
        EXPECT_TRUE(controller->getBindings().empty());
        EXPECT_EQ(captureDrawnText(draw).find("Delete shortcut"), std::string::npos);
    }

    TEST_F(BindingsListWidgetTests, SwitchingTheOwnerSelectionClearsTheOldShortcutEditor)
    {
        InputController::Ptr controller = new InputController;
        controller->setBindings({ { .action = "Old action"_atom, .chord = {} } });
        InputController::Ptr replacement = new InputController;
        Core::IntrusivePtr<BindingsTestOwner> owner = new BindingsTestOwner;
        owner->selectComponent(controller);
        TestInputBindingsEditor editor;
        editor.initialize();
        editor.setTarget(controller.get(), owner.get());
        const auto draw = [&editor] { editor.onDraw(); };
        EXPECT_NE(captureDrawnText(draw).find("Old action"), std::string::npos);

        owner->selectComponent(replacement);
        const auto cleared = captureDrawnText(draw);
        EXPECT_EQ(cleared.find("Old action"), std::string::npos);
        EXPECT_EQ(cleared.find("Delete shortcut"), std::string::npos);
        EXPECT_FALSE(owner->isDirty());
        ASSERT_EQ(controller->getBindings().size(), 1);
        EXPECT_EQ(controller->getBindings()[0].action, "Old action"_atom);
    }

    TEST_F(BindingsListWidgetTests, ExpiredControllerAndOwnerClearTheOldShortcutEditor)
    {
        TestInputBindingsEditor editor;
        editor.initialize();
        InputController::Ptr controller = new InputController;
        controller->setBindings({ { .action = "Old action"_atom, .chord = {} } });
        editor.setTarget(controller.get());
        const auto draw = [&editor] { editor.onDraw(); };
        EXPECT_NE(captureDrawnText(draw).find("Old action"), std::string::npos);
        controller.reset();
        EXPECT_EQ(captureDrawnText(draw).find("Old action"), std::string::npos);

        controller = new InputController;
        controller->setBindings({ { .action = "Other action"_atom, .chord = {} } });
        Core::IntrusivePtr<BindingsTestOwner> owner = new BindingsTestOwner;
        owner->selectComponent(controller);
        editor.setTarget(controller.get(), owner.get());
        EXPECT_NE(captureDrawnText(draw).find("Other action"), std::string::npos);
        owner.reset();
        EXPECT_EQ(captureDrawnText(draw).find("Other action"), std::string::npos);
        ASSERT_EQ(controller->getBindings().size(), 1);
        EXPECT_EQ(controller->getBindings()[0].action, "Other action"_atom);
    }

    TEST_F(BindingsListWidgetTests, InputAdapterShowsTheEditorButtonInsteadOfInlineSettings)
    {
        InputController::Ptr controller = new InputController;
        controller->setBindings({ { .action = "Jump"_atom, .chord = {} } });
        BindingsTestOwner owner;
        owner.selectComponent(controller);
        auto* adapter = owner.addChildComponent<ECSEditorInputControllerAdapter>();

        const auto text = captureDrawnText([adapter] { adapter->draw(0.f); });
        EXPECT_NE(text.find("Edit shortcuts"), std::string::npos);
        EXPECT_EQ(text.find("Record"), std::string::npos);
        EXPECT_EQ(text.find("Trigger"), std::string::npos);
        EXPECT_EQ(text.find("Delete shortcut"), std::string::npos);
    }
} // namespace
