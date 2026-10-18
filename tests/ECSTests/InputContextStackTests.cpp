// Nexium
// Copyright 2018-2026 Valerii Koniushenko
// Licensed under the Apache License, Version 2.0.

#include "NxWorld/Framework/Input/InputController.h"
#include "NxWorld/Framework/Input/InputSystem.h"
#include "Platform/Window.h"

#include "gtest/gtest.h"

using namespace NX;

TEST(InputContextStackTests, NewestOverlayTakesPriorityOnItsChannelsUntilClosed)
{
    auto& system = GetInputSystem();
    system.initialize(Platform::GetWindow());
    system.resetInput();
    system.setGlobalContext(std::nullopt);
    system.setActiveContext(InputContext::Gameplay);
    auto modal = InputController::Create("Modal"_atom, InputContext::Modal);
    auto text = InputController::Create("Text"_atom, InputContext::TextInput);
    modal->bind("Select"_atom, KeyChord::Exact(Platform::Keyboard::Key::W));
    modal->bind({ .action = "Scroll"_atom, .source = InputEventType::Wheel });
    text->bind("Type"_atom, KeyChord::Exact(Platform::Keyboard::Key::W));
    auto lower = system.pushContext(
        { .context = InputContext::Modal, .policy = InputContextPolicy::BlockLower });
    auto upper = system.pushContext({ .context = InputContext::TextInput,
                                      .policy = InputContextPolicy::BlockLower,
                                      .channels = InputChannel::Keyboard });

    Platform::GetWindow().onKeyPressed->trigger(Platform::Keyboard::Key::W, 0,
                                                Platform::Keyboard::KeyState::Pressed, 0);
    Platform::GetWindow().onMouseWheel->trigger(glm::vec2{ 0.f, 1.f });
    system.processEvents();
    EXPECT_TRUE(text->isActive("Type"_atom));
    EXPECT_FALSE(modal->isActive("Select"_atom));
    EXPECT_TRUE(modal->isActive("Scroll"_atom));

    upper.reset();
    Platform::GetWindow().onKeyPressed->trigger(Platform::Keyboard::Key::W, 0,
                                                Platform::Keyboard::KeyState::Released, 0);
    Platform::GetWindow().onKeyPressed->trigger(Platform::Keyboard::Key::W, 0,
                                                Platform::Keyboard::KeyState::Pressed, 0);
    system.processEvents();
    EXPECT_TRUE(modal->isActive("Select"_atom));
    EXPECT_FALSE(text->isActive("Type"_atom));
    system.resetInput();
}
