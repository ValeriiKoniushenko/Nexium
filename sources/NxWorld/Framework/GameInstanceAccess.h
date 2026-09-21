// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::World

#pragma once

namespace NX
{
    class AssetsManager;
    class Scene;
    class World;

    [[nodiscard]] World* GetWorld();
    [[nodiscard]] AssetsManager* GetAssetsManager();
    class SceneManager;
    [[nodiscard]] SceneManager* GetSceneManager();
    [[nodiscard]] Scene* GetGameScene();
    [[nodiscard]] bool IsEditorMode();

    void ResetCamera();
    void SaveAllToCache();
} // namespace NX
