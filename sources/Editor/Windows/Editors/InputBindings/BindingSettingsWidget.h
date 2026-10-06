// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Delegate.h"
#include "NxWorld/Framework/InputController.h"

#include <optional>
#include <string>
#include <vector>

namespace NX
{
    class BindingSettingsWidget
    {
    public:
        struct DrawResult
        {
            bool changed = false;
            bool deleteRequested = false;
        };

        [[nodiscard]] DrawResult draw(InputController::Binding* binding);

        void cancelRecording();

    private:
        [[nodiscard]] DrawResult drawSettings(InputController::Binding& binding);
        [[nodiscard]] bool drawAction(InputController::Binding& binding);
        [[nodiscard]] bool drawChord(InputController::Binding& binding);
        [[nodiscard]] bool drawTrigger(InputController::Binding& binding);

        void startRecording();
        void recordButton(InputButton button);
        void recordModifiers(int modifiers);
        void drawMouseCaptureArea();
        [[nodiscard]] bool isMouseInCaptureArea() const;

        [[nodiscard]] Platform::Keyboard::Key normalizeModifier(Platform::Keyboard::Key key);
        [[nodiscard]] bool isModifier(Platform::Keyboard::Key key);
        [[nodiscard]] Core::StringAtom buttonText(InputButton button);
        [[nodiscard]] Core::StringAtom chordText(const KeyChord& chord);
        [[nodiscard]] Core::StringAtom triggerText(InputActionTrigger trigger);

        void pushPanelStyle();
        void popPanelStyle();

    private:
        bool _recording = false;
        bool _cancellationRequested = false;
        std::optional<InputCapture> _inputCapture;
        KeyChord _recordedChord;
        std::vector<InputButton> _pressedButtons;
        bool _captureAreaValid = false;
        glm::vec2 _captureAreaMin{};
        glm::vec2 _captureAreaMax{};

        bool _actionBufferInitialized = false;
        Core::StringAtom _displayedAction;
        std::string _actionBuffer;

        Core::DelegateSubscriber _keyRecordingSubscription;
        Core::DelegateSubscriber _mouseRecordingSubscription;
        Core::DelegateSubscriber _focusRecordingSubscription;
    };
} // namespace NX
