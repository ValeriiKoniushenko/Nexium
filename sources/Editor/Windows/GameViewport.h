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

#include "BaseWindow.h"
#include "Editor/SceneGizmo.h"
#include "Scenes/SceneTabs/SceneTabs.h"

namespace NX
{
    CLASS();
    class GameViewportEWC : public BaseFloatEWC
    {
        ECS_DECL_NO_CNSTR(GameViewportEWC, NX::BaseFloatEWC);

    public:
        explicit GameViewportEWC(const StringAtom& name = ""_atom);

        [[nodiscard]] const char* getIcon() override;
        [[nodiscard]] bool blocksPicking() const noexcept { return _blocksPicking; }
        [[nodiscard]] glm::vec2 getImagePosition() const noexcept { return _imagePosition; }
        [[nodiscard]] FSize2 getImageSize() const noexcept { return _imageSize; }

        Delegate<void(FSize2)>::Ptr onImageSizeChanged = Delegate<void(FSize2)>::Create();

    protected:
        void onInitialize() override;
        void onOpen() override;
        void onClose() override;
        void onUpdate() override;
        void onDraw() override;
        [[nodiscard]] bool updateImageArea();

    private:
        glm::vec2 _imagePosition{};
        FSize2 _imageSize{};
        SceneGizmo _gizmo;
        bool _blocksPicking = false;
        SceneTabs _sceneTabs;
    };
} // namespace NX

#include "GameViewport.generated.h" // added by the code generator. Better don't move it.
