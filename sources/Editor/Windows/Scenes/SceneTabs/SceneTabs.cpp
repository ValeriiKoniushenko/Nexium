// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "SceneTabs.h"

#include "Editor/EditorIntegration.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/Combo.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/IconsFontAwesome.h"
#include "Foundation/Debug/Latency.h"
#include "ImGui/imgui_internal.h"
#include "NxWorld/Scene/SceneManager.h"
#include "SceneDeleteConfirmation.h"

#include <algorithm>

namespace NX
{
    struct SceneTabs::State
    {
        Gui::HorizontalLayout toolbar;
        Gui::SearchableComboBox* scenePicker = nullptr;
        Gui::Button trashButton;
        DelegateSubscriberPoolGuard subscriptions;
        WeakPtr<const Scene> actionScene;
    };

    namespace
    {
        StringAtom makeTabLabel(const Scene& scene)
        {
            constexpr float tabWidth = 150.f;
            constexpr std::size_t maxNameLength = 22;

            auto name = scene.getSceneName().toStdString();
            if (name.size() > maxNameLength)
            {
                name.resize(maxNameLength - 3);
                name += "...";
            }

            while (ImGui::CalcTextSize(name.c_str()).x < tabWidth)
            {
                name += ' ';
            }
            return StringAtom::Intern(name + "###scene");
        }
    } // namespace

    SceneTabs::SceneTabs()
        : _state(std::make_unique<State>())
    {
    }

    SceneTabs::~SceneTabs() = default;

    void SceneTabs::initialize(SceneManager& scenes)
    {
        if (_state->scenePicker)
        {
            return;
        }
        NX_LATENCY_POINT("Scene tabs initialization - start");
        _state->toolbar.setFitContent(true);
        _state->toolbar.setPaddings(4.f, 4.f, 2.f, 2.f);
        _state->toolbar.setHorizontalAlign(Gui::Align::Left);
        auto* sceneLabel = _state->toolbar.addChildComponent<Gui::Label>("Scenes"_atom);
        sceneLabel->setTextColor(Core::Color4_LightGray);
        sceneLabel->setWidth(54.f);
        _state->scenePicker
            = _state->toolbar.addChildComponent<Gui::SearchableComboBox>("Scenes"_atom);
        _state->scenePicker->setWidth(220.f);
        _state->scenePicker->setSizeProvider([&scenes] { return scenes.getScenes().size(); });
        _state->scenePicker->setDataProvider(
            [&scenes](std::size_t index, StringAtom& label) -> const void*
            {
                const auto* scene = scenes.getScenes()[index].get();
                label = scene->getSceneName();
                return scene;
            });
        _state->scenePicker->setSearchPlaceholder("Filter scenes..."_atom);
        _state->trashButton.setText(ICON_FA_TRASH ""_atom);
        _state->trashButton.setWidth(28.f);
        _state->trashButton.setButtonColor(Core::Color4(72, 52, 56, 255));
        _state->trashButton.setButtonHoverColor(Core::Color4(128, 60, 66, 255));
        _state->trashButton.setTextColor(Core::Color4(235, 150, 150, 255));
        _state->trashButton.initialize();
        _state->subscriptions << _state->trashButton.onClick->subscribeAndGetID(
            [this]
            {
                if (auto scene = _state->actionScene.tryLoad())
                {
                    GetEditor()->tryToOpenWindow<SceneDeleteConfirmation>(
                        ""_atom, const_cast<Scene*>(scene.get()));
                }
            });
        _state->scenePicker->setItemActionDrawer(
            [this, &scenes](const void* data)
            {
                _state->actionScene = static_cast<const Scene*>(data);
                _state->trashButton.disableWidget(scenes.getScenes().size() == 1);
                _state->trashButton.tick(0.f);
                _state->actionScene.reset();
            });
        _state->subscriptions << _state->scenePicker->onSelect->subscribeAndGetID(
            [&scenes](const void* data)
            { scenes.setCurrentScene(const_cast<Scene*>(static_cast<const Scene*>(data))); });
        auto* createButton
            = _state->toolbar.addChildComponent<Gui::Button>(ICON_FA_PLUS "  New"_atom);
        createButton->setWidth(76.f);
        createButton->setButtonColor(Core::Color4(54, 86, 68, 255));
        createButton->setButtonHoverColor(Core::Color4(70, 116, 88, 255));
        createButton->setTextColor(Core::Color4(185, 235, 195, 255));
        _state->subscriptions << createButton->onClick->subscribeAndGetID(
            [&scenes]
            {
                auto& scene = scenes.createNewScene();
                scenes.setCurrentScene(&scene);
            });

        _state->toolbar.initialize();
        NX_LATENCY_POINT("Scene tabs initialization - end");
    }

    void SceneTabs::draw(SceneManager& scenes)
    {
        drawScenePicker(scenes);
        drawTabs(scenes);
    }

    void SceneTabs::drawScenePicker(SceneManager& scenes)
    {
        const auto& allScenes = scenes.getScenes();
        const auto it = std::ranges::find_if(allScenes, [&scenes](const auto& scene)
                                             { return scene.get() == scenes.getCurrentScene(); });
        _state->scenePicker->setCurrentIndex(static_cast<std::size_t>(it - allScenes.begin()));
        _state->toolbar.tick(0.f);
    }

    void SceneTabs::drawTabs(SceneManager& scenes)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, glm::vec2(2.f, 0.f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
        ImGui::PushStyleColor(ImGuiCol_Tab, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
        ImGui::PushStyleColor(ImGuiCol_TabHovered,
                              ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered));
        ImGui::PushStyleColor(ImGuiCol_TabActive, glm::vec4(0.12f, 0.34f, 0.48f, 1.f));
        if (!ImGui::BeginTabBar("SceneTabs"_atom.c_str()))
        {
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(2);
            return;
        }
        Scene* requestedScene = nullptr;
        Scene* sceneToClose = nullptr;
        for (auto* scene : scenes.getOpenScenes())
        {
            ImGui::PushID(scene);
            const auto flags = scene == scenes.getCurrentScene() ? ImGuiTabItemFlags_SetSelected
                                                                 : ImGuiTabItemFlags_None;
            const auto label = makeTabLabel(*scene);
            bool keepOpen = true;
            const bool open = ImGui::BeginTabItem(
                label.c_str(), scenes.getOpenScenes().size() > 1 ? &keepOpen : nullptr, flags);
            if (!keepOpen)
            {
                sceneToClose = scene;
            }
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                requestedScene = scene;
            }
            if (open)
            {
                ImGui::EndTabItem();
            }
            ImGui::PopID();
        }
        ImGui::EndTabBar();
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);
        ImGui::Separator();
        if (requestedScene && requestedScene != sceneToClose)
        {
            scenes.setCurrentScene(requestedScene);
        }
        if (sceneToClose)
        {
            scenes.closeScene(sceneToClose);
        }
    }

} // namespace NX
