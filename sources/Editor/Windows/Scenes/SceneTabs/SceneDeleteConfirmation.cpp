// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "SceneDeleteConfirmation.h"

#include "NxWorld/Scene/Scene/SceneManager.h"

#include <algorithm>

namespace NX
{
    void SceneDeleteConfirmation::initialize(SceneManager& scenes)
    {
        _message.setWidth(360.f);
        _message.setHorizontalAlign(Gui::Align::Center);
        _message.setTruncateLongText(false);
        _message.initialize();

        _warning.setText("This cannot be undone."_atom);
        _warning.setWidth(360.f);
        _warning.setHorizontalAlign(Gui::Align::Center);
        _warning.setTextColor(Core::Color4_Gray);
        _warning.initialize();

        _buttons.setWidth(360.f);
        _buttons.setHorizontalAlign(Gui::Align::Center);
        _deleteButton = _buttons.addChildComponent<Gui::Button>("Delete"_atom);
        _deleteButton->setWidth(110.f);
        _deleteButton->setButtonColor(Core::Color4(116, 52, 58, 255));
        _deleteButton->setButtonHoverColor(Core::Color4(160, 64, 70, 255));
        _deleteButton->setTextColor(Core::Color4(255, 215, 215, 255));
        _subscriptions << _deleteButton->onClick->subscribeAndGetID(
            [this, &scenes]
            {
                scenes.removeScene(_pendingScene);
                _pendingScene = nullptr;
                ImGui::CloseCurrentPopup();
            });

        _cancelButton = _buttons.addChildComponent<Gui::Button>("Cancel"_atom);
        _cancelButton->setWidth(110.f);
        _subscriptions << _cancelButton->onClick->subscribeAndGetID(
            [this]
            {
                _pendingScene = nullptr;
                ImGui::CloseCurrentPopup();
            });
        _buttons.initialize();
    }

    void SceneDeleteConfirmation::draw(SceneManager& scenes)
    {
        if (_pendingScene
            && !std::ranges::any_of(scenes.getScenes(), [this](const auto& scene)
                                    { return scene.get() == _pendingScene; }))
        {
            _pendingScene = nullptr;
        }
        if (_pendingScene && !ImGui::IsPopupOpen("Delete scene?"_atom.c_str()))
        {
            ImGui::OpenPopup("Delete scene?"_atom.c_str());
        }

        ImGui::SetNextWindowSize(glm::vec2(420.f, 180.f), ImGuiCond_Appearing);
        if (!ImGui::BeginPopupModal("Delete scene?"_atom.c_str(), nullptr,
                                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
        {
            return;
        }
        if (!_pendingScene)
        {
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            return;
        }

        _message.setText("Delete scene '"_atom + _pendingScene->getSceneName()
                         + "' and all its objects?"_atom);
        _message.tick(0.f);
        ImGui::Dummy(glm::vec2(0.f, 8.f));
        _warning.tick(0.f);
        ImGui::Dummy(glm::vec2(0.f, 12.f));
        _deleteButton->disableWidget(scenes.getScenes().size() == 1);
        _buttons.tick(0.f);
        ImGui::Dummy(glm::vec2(0.f, 0.f));
        ImGui::EndPopup();
    }
} // namespace NX
