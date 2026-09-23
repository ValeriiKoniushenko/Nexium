// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Label.h"

namespace NX
{
    class Scene;
    class SceneManager;

    class SceneDeleteConfirmation final
    {
    public:
        void initialize(SceneManager& scenes);
        void request(Scene* scene) noexcept { _pendingScene = scene; }
        void draw(SceneManager& scenes);

    private:
        Gui::HorizontalLayout _buttons;
        Gui::Button* _deleteButton = nullptr;
        Gui::Button* _cancelButton = nullptr;
        Gui::Label _message;
        Gui::Label _warning;
        Core::DelegateSubscriberPoolGuard _subscriptions;
        Scene* _pendingScene = nullptr;
    };
} // namespace NX
