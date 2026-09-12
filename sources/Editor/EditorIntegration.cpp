/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 */

#include "EditorIntegration.h"

#include "Editor/Windows/GameViewport.h"
#include "Foundation/Configs.h"
#include "ImGui/imgui.h"
#include "NxWorld/Framework/GameInstance.h"

namespace
{
    Core::EditorIntegration* gEditorIntegration = nullptr;
}

namespace Core
{
    EditorIntegration::EditorIntegration(GameInstance& instance)
        : _instance(instance)
    {
        Assert(!gEditorIntegration, "Only one editor integration can be active.");
        gEditorIntegration = this;
        _instance.setApplicationIntegration(this);
    }

    EditorIntegration::~EditorIntegration()
    {
        if (_instance.getApplicationIntegration() == this)
        {
            _instance.setApplicationIntegration(nullptr);
        }
        gEditorIntegration = nullptr;
    }

    void EditorIntegration::initialize()
    {
        _editor.initialize();
    }

    void EditorIntegration::readFromCache()
    {
        _editor.readFromCache();
        ImGui::LoadIniSettingsFromDisk(Config::Path::imGuiWindowsIni.generic_string().c_str());
    }

    void EditorIntegration::writeToCache()
    {
        _editor.writeToCache();
        ImGui::SaveIniSettingsToDisk(Config::Path::imGuiWindowsIni.generic_string().c_str());
    }

    void EditorIntegration::updateInput()
    {
        _editor.keyboardInput.update();
    }

    void EditorIntegration::tick(float delta)
    {
        _editor.tick(delta);
    }

    void EditorIntegration::updateSceneInteraction(Scene& scene)
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
} // namespace Core
