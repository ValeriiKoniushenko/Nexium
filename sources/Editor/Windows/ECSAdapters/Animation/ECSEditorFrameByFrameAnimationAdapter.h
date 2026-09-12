// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once
#include "Core/Delegate.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "Editor/Windows/NxECSBasedEditor.h"
#include "FrameByFrameAnimationEditor.h"

#include <unordered_map>

namespace Core
{
    namespace Gui
    {
        class Button;
    }

    CLASS();
    class ECSEditorFrameByFrameAnimationAdapter : public ECSEditorMimeAdapter
    {
        ECS_DECL(ECSEditorFrameByFrameAnimationAdapter, Core::ECSEditorMimeAdapter);

    public:
        [[nodiscard]] bool canWorkWith(BaseComponent* component) const override;
        [[nodiscard]] StringAtom getProcessedAssetType() const override;

    protected:
        void onInitialize() override;
        void onDraw(float dt) override;
        void onApplyAssetData(const nlohmann::json&) override {}

    private:
        void drawAnimationCards();

        Gui::VerticalLayout _layout;
        Gui::Button* _addAnimButton = nullptr;
        DelegateSubscriberPoolGuard _subscriptionPool;
        FrameByFrameAnimationEditor _animationEditor;
        std::unordered_map<StringAtom, FrameByFrameAnimationEditor::PreviewState> _previewStates;
    };
} // namespace Core

#include "ECSEditorFrameByFrameAnimationAdapter.generated.h"
