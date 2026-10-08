// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "SceneDeleteConfirmation.h"

#include "NxWorld/Framework/GameInstanceAccess.h"
#include "NxWorld/Scene/Scene/SceneManager.h"

#include <algorithm>

namespace NX
{
    ECS_IMPL(SceneDeleteConfirmation);

    SceneDeleteConfirmation::SceneDeleteConfirmation(const StringAtom& name)
        : BaseModalPopUp(componentType, name)
    {
        _windowFlags |= ImGuiWindowFlags_NoResize;
    }

    void SceneDeleteConfirmation::open(Scene* scene)
    {
        if (!scene || _pendingScene)
        {
            return;
        }
        _pendingScene = scene;
        BaseModalPopUp::open("Delete scene?"_atom);
    }

    glm::vec2 SceneDeleteConfirmation::getInitialPopupSize() const
    {
        return glm::vec2(420.f, 180.f);
    }

    void SceneDeleteConfirmation::onOpen()
    {
        BaseModalPopUp::onOpen();
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
        _subscriptionPool << _deleteButton->onClick->subscribeAndGetID(
            [this]
            {
                if (auto scene = _pendingScene.tryLoad())
                {
                    GetSceneManager()->removeScene(scene.get());
                }
                closeConfirmation();
            });

        _cancelButton = _buttons.addChildComponent<Gui::Button>("Cancel"_atom);
        _cancelButton->setWidth(110.f);
        _subscriptionPool << _cancelButton->onClick->subscribeAndGetID([this]
                                                                       { closeConfirmation(); });
        _buttons.initialize();
    }

    void SceneDeleteConfirmation::onClose()
    {
        BaseModalPopUp::onClose();
        _pendingScene.reset();
        _hasOpenRequest = false;
        _buttons.removeAllChildren();
        _deleteButton = nullptr;
        _cancelButton = nullptr;
    }

    void SceneDeleteConfirmation::closeConfirmation()
    {
        ImGui::CloseCurrentPopup();
        closeWindow();
    }

    void SceneDeleteConfirmation::onDraw()
    {
        BaseModalPopUp::onDraw();
        auto& scenes = *GetSceneManager();
        auto pendingScene = _pendingScene.tryLoad();
        if (!pendingScene
            || !std::ranges::any_of(scenes.getScenes(), [&pendingScene](const auto& scene)
                                    { return scene.get() == pendingScene.get(); }))
        {
            closeConfirmation();
            return;
        }

        _message.setText("Delete scene '"_atom + pendingScene->getSceneName()
                         + "' and all its objects?"_atom);
        _message.tick(0.f);
        ImGui::Dummy(glm::vec2(0.f, 8.f));
        _warning.tick(0.f);
        ImGui::Dummy(glm::vec2(0.f, 12.f));
        _deleteButton->disableWidget(scenes.getScenes().size() == 1);
        _buttons.tick(0.f);
        ImGui::Dummy(glm::vec2(0.f, 0.f));
    }
} // namespace NX
