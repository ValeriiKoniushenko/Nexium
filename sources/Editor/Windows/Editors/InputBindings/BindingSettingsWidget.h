// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "NxWorld/Framework/InputController.h"

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

        void pollRecording();

        [[nodiscard]] Platform::Keyboard::Key normalizeModifier(Platform::Keyboard::Key key);
        [[nodiscard]] bool isModifier(Platform::Keyboard::Key key);
        [[nodiscard]] Core::StringAtom keyText(Platform::Keyboard::Key key);
        [[nodiscard]] Core::StringAtom chordText(const KeyChord& chord);
        [[nodiscard]] Core::StringAtom triggerText(InputActionTrigger trigger);

        void pushPanelStyle();
        void popPanelStyle();

    private:
        bool _recording = false;
        KeyChord _recordedChord;
        std::vector<Platform::Keyboard::Key> _recordedKeys;

        bool _actionBufferInitialized = false;
        Core::StringAtom _displayedAction;
        std::string _actionBuffer;
    };
} // namespace NX
