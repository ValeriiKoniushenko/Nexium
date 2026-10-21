// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "EditorIntegration.h"

#include "Editor/Windows/GameViewport.h"
#include "Foundation/Configs.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "NxRuntime/Runtime.h"
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Framework/Input/InputSystem.h"

namespace
{
    NX::EditorIntegration* gEditorIntegration = nullptr;
}

namespace NX
{
    EditorIntegration::EditorIntegration(NX::Runtime& instance)
        : _runtime(instance)
    {
        Assert(!gEditorIntegration, "Only one editor integration can be active.");
        gEditorIntegration = this;
        _runtime.getGameInstance().setApplicationIntegration(this);
    }

    EditorIntegration::~EditorIntegration()
    {
        if (_runtime.getGameInstance().getApplicationIntegration() == this)
        {
            _runtime.getGameInstance().setApplicationIntegration(nullptr);
        }
        gEditorIntegration = nullptr;
    }

    void EditorIntegration::preInitialize()
    {
        _editor.preInitialize();
    }

    void EditorIntegration::initialize()
    {
        _editor.initialize();
    }

    void EditorIntegration::readFromCache()
    {
        _editor.readFromCache();
        ImGui::LoadIniSettingsFromDisk(
            Foundation::Config::Path::imGuiWindowsIni.generic_string().c_str());
    }

    void EditorIntegration::writeToCache()
    {
        _editor.writeToCache();
        ImGui::SaveIniSettingsToDisk(
            Foundation::Config::Path::imGuiWindowsIni.generic_string().c_str());
    }

    // tag::input-editor-routing[]
    void EditorIntegration::updateInputRouting()
    {
        auto& input = GetInputSystem();
        const bool editorVisible = _editor.isEnabled() && _runtime.getGameInstance().isEditorMode();
        const auto* viewport = _editor.getWindow<GameViewportEWC>();
        const bool viewportFocused = editorVisible && viewport && viewport->isFocused();
        const bool viewportHovered = editorVisible && viewport && viewport->isHovered();
        input.setActiveContext(
            editorVisible ? (viewportFocused ? InputContext::EditorViewport : InputContext::Editor)
                          : InputContext::Gameplay);
        input.setGlobalContext(_editor.isEnabled() ? std::optional{ InputContext::Editor }
                                                   : std::nullopt);

        const auto setOverlay =
            [&input](std::optional<InputContextGuard>& guard, bool enabled, InputContextLayer layer)
        {
            if (enabled && !guard)
            {
                guard.emplace(input.pushContext(layer));
            }
            else if (!enabled)
            {
                guard.reset();
            }
        };
        const auto& io = ImGui::GetIO();
        const bool modalOpen = editorVisible && ImGui::GetTopMostPopupModal();
        if (modalOpen)
        {
            _viewportHoverContext.reset();
            _textInputContext.reset();
            _pointerCaptureContext.reset();
            setOverlay(
                _modalContext, true,
                { .context = InputContext::Modal, .policy = InputContextPolicy::BlockLower });
            return;
        }
        _modalContext.reset();
        const bool pointerCaptured = input.hasPointerCapture();
        setOverlay(_viewportHoverContext,
                   editorVisible && (viewportHovered || pointerCaptured) && !viewportFocused,
                   { .context = InputContext::EditorViewport, .channels = InputChannel_Pointer });
        setOverlay(_textInputContext,
                   editorVisible
                       && (io.WantTextInput || (io.WantCaptureKeyboard && !viewportFocused)),
                   { .context = InputContext::TextInput,
                     .policy = InputContextPolicy::BlockLower,
                     .channels = InputChannel_Keyboard });
        setOverlay(_pointerCaptureContext,
                   editorVisible && io.WantCaptureMouse && !viewportHovered && !pointerCaptured,
                   { .context = InputContext::TextInput,
                     .policy = InputContextPolicy::BlockLower,
                     .channels = InputChannel_Pointer });
    }
    // end::input-editor-routing[]

    void EditorIntegration::updateInput()
    {
    }

    void EditorIntegration::tick(float delta)
    {
        _editor.tick(delta);
    }

    void EditorIntegration::updateSceneInteraction(NX::Scene& scene)
    {
        _objectPicker.update(scene);
    }

    bool EditorIntegration::isViewportFocused() const
    {
        const auto* viewport = _editor.getWindow<GameViewportEWC>();
        return viewport && viewport->isFocused();
    }

    void EditorIntegration::beforeSceneDraw()
    {
        _editor.gameViewport.callMePreDraw();
    }

    void EditorIntegration::afterSceneDraw()
    {
        _editor.gameViewport.callMeAfterDraw();
    }

    void EditorIntegration::clearSceneRenderTarget()
    {
        _editor.gameViewport.clearCanvas();
    }

    ISize2 EditorIntegration::getRenderSize() const
    {
        return _editor.gameViewport.getRenderSize();
    }

    EditorIntegration* GetEditorIntegration()
    {
        return gEditorIntegration;
    }

    GameEditor* GetEditor()
    {
        return gEditorIntegration ? &gEditorIntegration->getEditor() : nullptr;
    }

    ObjectSelectorManager* GetObjectSelectorManager()
    {
        return gEditorIntegration ? &gEditorIntegration->getObjectSelectorManager() : nullptr;
    }

    ObjectPickerAggregator* GetObjectPicker()
    {
        return gEditorIntegration ? &gEditorIntegration->getObjectPicker() : nullptr;
    }
} // namespace NX
