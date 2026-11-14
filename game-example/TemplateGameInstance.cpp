// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "TemplateGameInstance.h"

#include "Player.h"

#include <algorithm>

using namespace NX;

TemplateGameInstance::TemplateGameInstance(int argc, char** argv)
    : GameInstance(argc, argv)
{
}

void TemplateGameInstance::onInitializeReadCache()
{
    auto* scene = scenes.getCurrentScene();
    if (!scene)
    {
        return;
    }

    const auto hasPlayer = std::ranges::any_of(
        scene->getObjects(), [](const auto& object) { return dynamic_cast<Player*>(object.get()); });
    if (hasPlayer)
    {
        return;
    }

    scene->addObjectToScene(Player::Create());
    infoLog("Spawned a Player in the current scene");
}
