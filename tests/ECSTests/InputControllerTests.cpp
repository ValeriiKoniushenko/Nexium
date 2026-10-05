// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/InputController.h"
#include "NxWorld/Framework/InputSystem.h"
#include "Platform/Window.h"

#include "gtest/gtest.h"
#include <array>

using namespace NX;
using Key = Platform::Keyboard::Key;
using State = Platform::Keyboard::KeyState;
using MouseButton = Platform::Mouse::Key;
using MouseState = Platform::Mouse::State;

namespace
{
    class InputControllerTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            GetInputSystem().initialize(Platform::GetWindow());
            GetInputSystem().resetInput();
            GetInputSystem().setActiveContext(InputContext::Gameplay);
        }

        void TearDown() override { GetInputSystem().resetInput(); }

        static void key(Key key, State state, int modifiers = 0)
        {
            Platform::GetWindow().onKeyPressed->trigger(key, 0, state, modifiers);
        }

        static void mouse(MouseButton button, MouseState state, int modifiers = 0)
        {
            Platform::GetWindow().onMouseKeyPressed->trigger(
                button, state, static_cast<Platform::Mouse::Mod>(modifiers));
        }

        static void frame() { GetInputSystem().processEvents(); }
    };
} // namespace

TEST_F(InputControllerTests, RoutesContextsAndReleasesHeldActions)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    auto editor = InputController::Create("Editor"_atom, InputContext::Editor);
    game->bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    editor->bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(game->isActionPressed("Move"_atom));
    EXPECT_FALSE(editor->isActionPressed("Move"_atom));
    frame();
    EXPECT_TRUE(game->isActionPressed("Move"_atom));
    GetInputSystem().setActiveContext(InputContext::Editor);
    EXPECT_FALSE(game->isActionPressed("Move"_atom));
    key(Key::W, State::Released);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(editor->isActionPressed("Move"_atom));
}

TEST_F(InputControllerTests, MostSpecificChordWinsAndRightModifiersWork)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    int saved = 0;
    controller->bind("Plain"_atom, KeyChord::Exact(Key::S));
    controller->bind(
        "Save"_atom,
        { .triggerKey = Key::S, .requiredKeys = { Key::Left_Control, Key::Left_Shift } },
        [&](const InputActionEvent&) { ++saved; });
    key(Key::Right_Control, State::Pressed);
    key(Key::Right_Shift, State::Pressed);
    key(Key::S, State::Pressed, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT);
    frame();
    EXPECT_EQ(saved, 1);
    EXPECT_TRUE(controller->isActionPressed("Save"_atom));
    EXPECT_FALSE(controller->isActionPressed("Plain"_atom));
    key(Key::S, State::Repeated);
    frame();
    EXPECT_EQ(saved, 1);
    EXPECT_FALSE(controller->isActionPressed("Save"_atom));
}

TEST_F(InputControllerTests, ReleaseTriggerLastsOneFrame)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->bind("Release"_atom, KeyChord::Exact(Key::W), InputActionTrigger::OnRelease);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Release"_atom));
    key(Key::W, State::Released);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Release"_atom));
    frame();
    EXPECT_FALSE(controller->isActionPressed("Release"_atom));
}

TEST_F(InputControllerTests, DisableAndReenableDoesNotLeaveStuckKeys)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    key(Key::W, State::Pressed);
    frame();
    controller->disable();
    key(Key::W, State::Released);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Move"_atom));
    controller->enable();
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Move"_atom));
    GetInputSystem().resetInput();
    EXPECT_FALSE(controller->isActionPressed("Move"_atom));
}

TEST_F(InputControllerTests, BindingSerializationPreservesChordAndTrigger)
{
    const InputController::Binding binding{ .action = "Save"_atom,
                                            .chord = { .triggerKey = Key::S,
                                                       .requiredKeys = { Key::Left_Control } },
                                            .trigger = InputActionTrigger::OnRelease };
    const nlohmann::json json = binding;
    const auto restored = json.get<InputController::Binding>();
    EXPECT_EQ(restored.action, binding.action);
    EXPECT_EQ(restored.chord.triggerKey, Key::S);
    EXPECT_EQ(restored.chord.requiredKeys, binding.chord.requiredKeys);
    EXPECT_EQ(restored.trigger, binding.trigger);
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->setBindings({ restored });
    EXPECT_TRUE(controller->getTags() & Tag_InputController);
    EXPECT_EQ(TagHelper::ToTag("InputController"), Tag_InputController);
    EXPECT_EQ(controller->serialize()["_bindings"][0], json);
}

TEST_F(InputControllerTests, CallbackCanDestroyAnotherRegisteredController)
{
    auto first = InputController::Create("First"_atom, InputContext::Gameplay);
    auto second = InputController::Create("Second"_atom, InputContext::Gameplay);
    int calls = 0;
    first->bind("Delete"_atom, KeyChord::Exact(Key::W),
                [&](const InputActionEvent&) { second = nullptr; });
    second->bind("Move"_atom, KeyChord::Exact(Key::W), [&](const InputActionEvent&) { ++calls; });
    key(Key::W, State::Pressed);
    frame();
    EXPECT_EQ(calls, 0);
}

TEST_F(InputControllerTests, EditingBindingsPreservesCallbacksAndResetsHeldState)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    int calls = 0;
    controller->bind(
        "Move"_atom, KeyChord::Exact(Key::W), [&](const InputActionEvent&) { ++calls; },
        InputActionTrigger::WhileHeld);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_EQ(calls, 1);
    controller->setBindings(controller->getBindings());
    EXPECT_FALSE(controller->isActionPressed("Move"_atom));
    ASSERT_EQ(controller->getBindings().size(), 1);
    key(Key::W, State::Released);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_EQ(calls, 2);
}

TEST_F(InputControllerTests, CloneHasIndependentRuntimeStateAndRegistersOnInitialize)
{
    auto original = InputController::Create("Original"_atom, InputContext::Gameplay);
    original->bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    key(Key::W, State::Pressed);
    frame();
    auto clonedBase = original->clone();
    auto* clone = dynamic_cast<InputController*>(clonedBase.get());
    ASSERT_NE(clone, nullptr);
    EXPECT_FALSE(clone->isActionPressed("Move"_atom));
    EXPECT_NE(clone->onAction.get(), original->onAction.get());
    clone->initialize();
    key(Key::W, State::Released);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(clone->isActionPressed("Move"_atom));
}

TEST_F(InputControllerTests, DeserializedComponentReceivesInput)
{
    InputController original;
    original.bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    RResourceStream<RJsonResourceStream> stream(original.serialize());
    auto restored = InputController::Create();
    restored->deserialize(stream);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(restored->isActionPressed("Move"_atom));
    restored->deserialize(stream);
    EXPECT_FALSE(restored->isActionPressed("Move"_atom));
}

TEST_F(InputControllerTests, FocusLossClearsHeldAndQueuedInputEvenIfFocusReturnsBeforeNextFrame)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    controller->bind("Queued"_atom, KeyChord::Exact(Key::S));
    key(Key::W, State::Pressed);
    frame();
    ASSERT_TRUE(controller->isActionPressed("Move"_atom));
    key(Key::S, State::Pressed);
    Platform::GetWindow().onFocusChanged->trigger(false);
    EXPECT_FALSE(controller->isActionPressed("Move"_atom));
    Platform::GetWindow().onFocusChanged->trigger(true);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Queued"_atom));
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Move"_atom));
}

TEST_F(InputControllerTests, ReleasingRequiredKeyEndsHeldChord)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->bind("Move"_atom, { .triggerKey = Key::W, .requiredKeys = { Key::Left_Control } },
                     InputActionTrigger::WhileHeld);
    key(Key::Left_Control, State::Pressed);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Move"_atom));
    key(Key::Left_Control, State::Released);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Move"_atom));
}

TEST_F(InputControllerTests, RoutesAllEightMouseButtons)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    constexpr std::array buttons{ MouseButton::Left, MouseButton::Right, MouseButton::Middle,
                                  MouseButton::_4,   MouseButton::_5,    MouseButton::_6,
                                  MouseButton::_7,   MouseButton::_8 };
    int calls = 0;
    for (const auto button : buttons)
    {
        controller->bind("Click"_atom, KeyChord::Exact(button),
                         [&](const InputActionEvent& event)
                         {
                             EXPECT_TRUE(event.isPressed());
                             ++calls;
                         });
        mouse(button, MouseState::Press);
        frame();
        EXPECT_TRUE(controller->isActionPressed("Click"_atom));
        mouse(button, MouseState::Release);
        frame();
        EXPECT_FALSE(controller->isActionPressed("Click"_atom));
    }
    EXPECT_EQ(calls, buttons.size());
}

TEST_F(InputControllerTests, MouseAndKeyboardCodesRemainDistinct)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->bind("Click"_atom, KeyChord::Exact(MouseButton::Left));
    const auto overlappingKeyboardCode = static_cast<Key>(static_cast<int>(MouseButton::Left));
    key(overlappingKeyboardCode, State::Pressed);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Click"_atom));
    mouse(MouseButton::Left, MouseState::Press);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Click"_atom));
}

TEST_F(InputControllerTests, RejectsEmptyKeyboardAndMouseTriggers)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    EXPECT_FALSE(controller->bind("Keyboard"_atom, KeyChord::Exact(Key::None)));
    EXPECT_FALSE(controller->bind("Mouse"_atom, KeyChord::Exact(MouseButton::None),
                                  [](const InputActionEvent&) {}));
    EXPECT_TRUE(controller->getBindings().empty());
}

TEST_F(InputControllerTests, MostSpecificMouseChordWinsAndRightModifiersWork)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->bind("Plain"_atom, KeyChord::Exact(MouseButton::Left));
    controller->bind("Modified"_atom, { .triggerKey = MouseButton::Left,
                                        .requiredKeys = { Key::Left_Control, Key::Left_Shift } });
    key(Key::Right_Control, State::Pressed);
    key(Key::Right_Shift, State::Pressed);
    mouse(MouseButton::Left, MouseState::Press, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Modified"_atom));
    EXPECT_FALSE(controller->isActionPressed("Plain"_atom));
    EXPECT_EQ(controller->getActionModifiers("Modified"_atom),
              InputModifier::Control | InputModifier::Shift);
}

TEST_F(InputControllerTests, KeyboardTriggerCanRequireMouseAndKeyboardButtons)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->bind(
        "Move"_atom,
        { .triggerKey = Key::W, .requiredKeys = { MouseButton::Right, Key::Left_Control } },
        InputActionTrigger::WhileHeld);
    key(Key::Left_Control, State::Pressed);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Move"_atom));
    key(Key::W, State::Released);
    mouse(MouseButton::Right, MouseState::Press);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Move"_atom));
    mouse(MouseButton::Right, MouseState::Release);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Move"_atom));
}

TEST_F(InputControllerTests, MouseTriggerCanRequireAnotherMouseButtonAndKeyboard)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->bind("Drag"_atom,
                     { .triggerKey = MouseButton::Left,
                       .requiredKeys = { MouseButton::Right, Key::Left_Control } },
                     InputActionTrigger::WhileHeld);
    mouse(MouseButton::Right, MouseState::Press);
    mouse(MouseButton::Left, MouseState::Press);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Drag"_atom));
    mouse(MouseButton::Left, MouseState::Release);
    key(Key::Left_Control, State::Pressed);
    mouse(MouseButton::Left, MouseState::Press);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Drag"_atom));
    key(Key::Left_Control, State::Released);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Drag"_atom));
}

TEST_F(InputControllerTests, MouseReleaseTriggerLastsOneFrame)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    int released = 0;
    controller->bind(
        "Release"_atom, KeyChord::Exact(MouseButton::Middle),
        [&](const InputActionEvent& event)
        {
            EXPECT_TRUE(event.isReleased());
            ++released;
        },
        InputActionTrigger::OnRelease);
    mouse(MouseButton::Middle, MouseState::Press);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Release"_atom));
    EXPECT_EQ(released, 0);
    mouse(MouseButton::Middle, MouseState::Release);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Release"_atom));
    EXPECT_EQ(released, 1);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Release"_atom));
}

TEST_F(InputControllerTests, RoutesMouseContextsAndReleasesHeldActions)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    auto editor = InputController::Create("Editor"_atom, InputContext::Editor);
    game->bind("Drag"_atom, KeyChord::Exact(MouseButton::Right), InputActionTrigger::WhileHeld);
    editor->bind("Drag"_atom, KeyChord::Exact(MouseButton::Right), InputActionTrigger::WhileHeld);
    mouse(MouseButton::Right, MouseState::Press);
    frame();
    EXPECT_TRUE(game->isActionPressed("Drag"_atom));
    EXPECT_FALSE(editor->isActionPressed("Drag"_atom));
    frame();
    EXPECT_TRUE(game->isActionPressed("Drag"_atom));
    GetInputSystem().setActiveContext(InputContext::Editor);
    EXPECT_FALSE(game->isActionPressed("Drag"_atom));
    mouse(MouseButton::Right, MouseState::Release);
    mouse(MouseButton::Right, MouseState::Press);
    frame();
    EXPECT_TRUE(editor->isActionPressed("Drag"_atom));
    mouse(MouseButton::Right, MouseState::Release);
    frame();
    EXPECT_FALSE(editor->isActionPressed("Drag"_atom));
}

TEST_F(InputControllerTests, FocusLossClearsHeldAndQueuedMouseInput)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->bind("Drag"_atom,
                     { .triggerKey = MouseButton::Right, .requiredKeys = { Key::Left_Control } },
                     InputActionTrigger::WhileHeld);
    controller->bind("Queued"_atom, KeyChord::Exact(MouseButton::Left));
    key(Key::Left_Control, State::Pressed);
    mouse(MouseButton::Right, MouseState::Press);
    frame();
    ASSERT_TRUE(controller->isActionPressed("Drag"_atom));
    mouse(MouseButton::Left, MouseState::Press);
    Platform::GetWindow().onFocusChanged->trigger(false);
    EXPECT_FALSE(controller->isActionPressed("Drag"_atom));
    Platform::GetWindow().onFocusChanged->trigger(true);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Queued"_atom));
    mouse(MouseButton::Right, MouseState::Press);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Drag"_atom));
    key(Key::Left_Control, State::Pressed);
    mouse(MouseButton::Right, MouseState::Release);
    mouse(MouseButton::Right, MouseState::Press);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Drag"_atom));
}

TEST_F(InputControllerTests, MouseSerializationPreservesMixedChordAndTrigger)
{
    constexpr std::array buttons{ MouseButton::Left, MouseButton::Right, MouseButton::Middle,
                                  MouseButton::_4,   MouseButton::_5,    MouseButton::_6,
                                  MouseButton::_7,   MouseButton::_8 };
    const std::array names{ "Left"_atom, "Right"_atom, "Middle"_atom, "_4"_atom,
                            "_5"_atom,   "_6"_atom,    "_7"_atom,     "_8"_atom };
    for (std::size_t i = 0; i < buttons.size(); ++i)
    {
        const InputController::Binding binding{ .action = "Drag"_atom,
                                                .chord = { .triggerKey = buttons[i],
                                                           .requiredKeys = { Key::Left_Control } },
                                                .trigger = InputActionTrigger::WhileHeld };
        const nlohmann::json json = binding;
        EXPECT_EQ(json["triggerKey"],
                  (nlohmann::json{ { "device", "Mouse" }, { "button", names[i].toStdString() } }));
        EXPECT_EQ(json["requiredKeys"][0], "Left_Control");
        const auto restored = json.get<InputController::Binding>();
        EXPECT_EQ(restored.action, binding.action);
        EXPECT_EQ(restored.chord.triggerKey, binding.chord.triggerKey);
        EXPECT_EQ(restored.chord.requiredKeys, binding.chord.requiredKeys);
        EXPECT_EQ(restored.trigger, binding.trigger);
    }

    const InputController::Binding binding{
        .action = "Move"_atom,
        .chord = { .triggerKey = Key::W, .requiredKeys = { MouseButton::Right, Key::Left_Control } }
    };
    const nlohmann::json json = binding;
    EXPECT_EQ(json["triggerKey"], "W");
    EXPECT_EQ(json["requiredKeys"][0],
              (nlohmann::json{ { "device", "Mouse" }, { "button", "Right" } }));
    const auto restored = json.get<InputController::Binding>();
    EXPECT_EQ(restored.chord.triggerKey, binding.chord.triggerKey);
    EXPECT_EQ(restored.chord.requiredKeys, binding.chord.requiredKeys);
}

TEST_F(InputControllerTests, MouseSupportPreservesLegacyKeyboardSerialization)
{
    const nlohmann::json legacy{
        { "action", "Save" },
        { "key", static_cast<int>(Key::S) },
        { "requiredKeys", { "Left_Shift", static_cast<int>(Key::Left_Alt) } },
        { "requiredModifiers", static_cast<int>(InputModifier::Control) }
    };
    const auto restored = legacy.get<InputController::Binding>();
    EXPECT_EQ(restored.action, "Save"_atom);
    EXPECT_EQ(restored.chord.triggerKey, Key::S);
    EXPECT_EQ(restored.chord.requiredKeys,
              (std::vector<InputButton>{ Key::Left_Shift, Key::Left_Alt, Key::Left_Control }));
    EXPECT_EQ(restored.trigger, InputActionTrigger::OnPress);
    const nlohmann::json serialized = restored;
    EXPECT_EQ(serialized["triggerKey"], "S");
    EXPECT_EQ(serialized["requiredKeys"],
              (nlohmann::json::array({ "Left_Shift", "Left_Alt", "Left_Control" })));
}

TEST_F(InputControllerTests, DeserializedMouseComponentReceivesInput)
{
    InputController original;
    original.bind("Drag"_atom, KeyChord::Exact(MouseButton::_4), InputActionTrigger::WhileHeld);
    RResourceStream<RJsonResourceStream> stream(original.serialize());
    auto restored = InputController::Create();
    restored->deserialize(stream);
    mouse(MouseButton::_4, MouseState::Press);
    frame();
    EXPECT_TRUE(restored->isActionPressed("Drag"_atom));
    restored->deserialize(stream);
    EXPECT_FALSE(restored->isActionPressed("Drag"_atom));
}

TEST_F(InputControllerTests, CaptureSuppressesMouseChordUntilRecordedButtonsAreReleased)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    int clicked = 0;
    int confirmed = 0;
    controller->bind("ModifiedClick"_atom,
                     { .triggerKey = MouseButton::Left, .requiredKeys = { Key::Left_Control } },
                     [&](const InputActionEvent&) { ++clicked; });
    controller->bind(
        "Confirm"_atom, KeyChord::Exact(Key::Enter), [&](const InputActionEvent&) { ++confirmed; },
        InputActionTrigger::OnRelease);
    {
        InputCapture capture;
        key(Key::Right_Control, State::Pressed);
        mouse(MouseButton::Left, MouseState::Press, GLFW_MOD_CONTROL);
        key(Key::Enter, State::Pressed);
        frame();
        EXPECT_FALSE(controller->isActionPressed("ModifiedClick"_atom));
        EXPECT_EQ(clicked, 0);
        EXPECT_EQ(confirmed, 0);
    }
    EXPECT_TRUE(InputCapture::isActive());
    mouse(MouseButton::Left, MouseState::Release);
    key(Key::Right_Control, State::Released);
    frame();
    EXPECT_TRUE(InputCapture::isActive());
    key(Key::Enter, State::Released);
    frame();
    EXPECT_FALSE(InputCapture::isActive());
    EXPECT_EQ(clicked, 0);
    EXPECT_EQ(confirmed, 0);

    key(Key::Right_Control, State::Pressed);
    mouse(MouseButton::Left, MouseState::Press, GLFW_MOD_CONTROL);
    frame();
    EXPECT_TRUE(controller->isActionPressed("ModifiedClick"_atom));
    EXPECT_EQ(clicked, 1);
    key(Key::Enter, State::Pressed);
    key(Key::Enter, State::Released);
    frame();
    EXPECT_EQ(confirmed, 1);
}

TEST_F(InputControllerTests, CaptureReleasesHeldAndPendingReleaseActionsWithoutNewEvents)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    int released = 0;
    controller->bind("Drag"_atom, KeyChord::Exact(MouseButton::Right),
                     InputActionTrigger::WhileHeld);
    controller->bind(
        "Release"_atom, KeyChord::Exact(Key::W), [&](const InputActionEvent&) { ++released; },
        InputActionTrigger::OnRelease);
    mouse(MouseButton::Right, MouseState::Press);
    key(Key::W, State::Pressed);
    frame();
    ASSERT_TRUE(controller->isActionPressed("Drag"_atom));
    {
        InputCapture capture;
        frame();
        EXPECT_FALSE(controller->isActionPressed("Drag"_atom));
        mouse(MouseButton::Right, MouseState::Release);
        key(Key::W, State::Released);
        frame();
        EXPECT_EQ(released, 0);
    }
    EXPECT_FALSE(InputCapture::isActive());
    mouse(MouseButton::Right, MouseState::Press);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Drag"_atom));
}

TEST_F(InputControllerTests, CaptureWaitsForBothModifierSidesToBeReleased)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    int clicked = 0;
    controller->bind("ModifiedClick"_atom,
                     { .triggerKey = MouseButton::Left, .requiredKeys = { Key::Left_Control } },
                     [&](const InputActionEvent&) { ++clicked; });
    {
        InputCapture capture;
        key(Key::Left_Control, State::Pressed);
        key(Key::Right_Control, State::Pressed);
        mouse(MouseButton::Left, MouseState::Press, GLFW_MOD_CONTROL);
        frame();
    }
    ASSERT_TRUE(InputCapture::isActive());
    key(Key::Left_Control, State::Released);
    mouse(MouseButton::Left, MouseState::Release);
    frame();
    EXPECT_TRUE(InputCapture::isActive());
    EXPECT_EQ(clicked, 0);
    key(Key::Right_Control, State::Released);
    frame();
    EXPECT_FALSE(InputCapture::isActive());
    EXPECT_EQ(clicked, 0);

    key(Key::Right_Control, State::Pressed);
    mouse(MouseButton::Left, MouseState::Press, GLFW_MOD_CONTROL);
    frame();
    EXPECT_EQ(clicked, 1);
}

TEST_F(InputControllerTests, FocusLossClearsInputCaptureReleaseFence)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->bind("Click"_atom, KeyChord::Exact(MouseButton::Left));
    {
        InputCapture capture;
        mouse(MouseButton::Left, MouseState::Press);
        frame();
    }
    ASSERT_TRUE(InputCapture::isActive());
    Platform::GetWindow().onFocusChanged->trigger(false);
    EXPECT_FALSE(InputCapture::isActive());
    Platform::GetWindow().onFocusChanged->trigger(true);
    mouse(MouseButton::Left, MouseState::Press);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Click"_atom));
}
