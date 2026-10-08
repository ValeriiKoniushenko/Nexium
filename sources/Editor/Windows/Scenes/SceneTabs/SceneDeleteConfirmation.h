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
#include "Editor/Windows/BaseWindow.h"
#include "NxWorld/Scene/Scene/Scene.h"

namespace NX
{
    class Scene;
    class SceneManager;

    CLASS();
    class SceneDeleteConfirmation final : public BaseModalPopUp
    {
        ECS_DECL_NO_CNSTR(SceneDeleteConfirmation, NX::BaseModalPopUp);

    public:
        explicit SceneDeleteConfirmation(const StringAtom& name = ""_atom);
        void open(Scene* scene);

    protected:
        void onOpen() override;
        void onClose() override;
        void onDraw() override;
        [[nodiscard]] glm::vec2 getInitialPopupSize() const override;

    private:
        void closeConfirmation();

    private:
        Gui::HorizontalLayout _buttons;
        Gui::Button* _deleteButton = nullptr;
        Gui::Button* _cancelButton = nullptr;
        Gui::Label _message;
        Gui::Label _warning;
        Core::WeakPtr<Scene> _pendingScene;
    };
} // namespace NX

#include "SceneDeleteConfirmation.generated.h" // added by the code generator. Better don't move it.
