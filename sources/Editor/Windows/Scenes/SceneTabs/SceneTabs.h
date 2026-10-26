// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Editor

#pragma once

#include <memory>

namespace NX
{
    class SceneManager;

    class SceneTabs final
    {
    public:
        SceneTabs();
        ~SceneTabs();

        void initialize(SceneManager& scenes);
        void draw(SceneManager& scenes);

    private:
        void drawScenePicker(SceneManager& scenes);
        void drawTabs(SceneManager& scenes);

        struct State;
        std::unique_ptr<State> _state;
    };
} // namespace NX
