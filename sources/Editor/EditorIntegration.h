// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "GameEditor.h"
#include "NxWorld/Framework/ApplicationIntegration.h"
#include "ObjectPicker.h"
#include "ObjectSelectorManager.h"

namespace Core
{
    class GameInstance;

    class EditorIntegration final : public ApplicationIntegration
    {
    public:
        explicit EditorIntegration(GameInstance& instance);
        ~EditorIntegration() override;

        void initialize() override;
        void readFromCache() override;
        void writeToCache() override;
        void updateInput() override;
        void tick(float delta) override;
        void updateSceneInteraction(Scene& scene) override;
        [[nodiscard]] bool isViewportFocused() const override;
        void beforeSceneDraw() override;
        void afterSceneDraw() override;
        void clearSceneRenderTarget() override;
        [[nodiscard]] ISize2 getRenderSize() const override;

        [[nodiscard]] GameEditor& getEditor() noexcept { return _editor; }
        [[nodiscard]] ObjectSelectorManager& getObjectSelectorManager() noexcept
        {
            return _objectSelectorManager;
        }
        [[nodiscard]] ObjectPickerAggregator& getObjectPicker() noexcept { return _objectPicker; }

    private:
        GameInstance& _instance;
        GameEditor _editor;
        ObjectSelectorManager _objectSelectorManager;
        ObjectPickerAggregator _objectPicker;
    };

    [[nodiscard]] EditorIntegration* GetEditorIntegration();
    [[nodiscard]] GameEditor* GetEditor();
    [[nodiscard]] ObjectSelectorManager* GetObjectSelectorManager();
    [[nodiscard]] ObjectPickerAggregator* GetObjectPicker();
} // namespace Core
