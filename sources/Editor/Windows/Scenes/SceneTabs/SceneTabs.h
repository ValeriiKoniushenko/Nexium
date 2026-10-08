// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Label.h"
#include "NxWorld/Scene/Scene/Scene.h"

namespace NX
{
    class SceneManager;
    class Scene;

    namespace Gui
    {
        class Label;
        class HorizontalLayout;
        class Button;
        class SearchableComboBox;
    } // namespace Gui

    class SceneTabs final
    {
    public:
        void initialize(SceneManager& scenes);
        void draw(SceneManager& scenes);

    private:
        void drawScenePicker(SceneManager& scenes);
        void drawTabs(SceneManager& scenes);

        Gui::HorizontalLayout _toolbar;
        Gui::Label* _sceneLabel = nullptr;
        Gui::SearchableComboBox* _scenePicker = nullptr;
        Gui::Button _trashButton;
        DelegateSubscriberPoolGuard _subscriptions;
        WeakPtr<const Scene> _actionScene;
    };
} // namespace NX
