// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "SceneTabs.h"

#include "Editor/IconsFontAwesome.h"
#include "ImGui/imgui_internal.h"
#include "NxWorld/Scene/Scene/SceneManager.h"

#include <algorithm>

namespace NX
{
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

    void SceneTabs::initialize(SceneManager& scenes)
    {
        if (_scenePicker)
        {
            return;
        }
        _toolbar.setFitContent(true);
        _toolbar.setPaddings(4.f, 4.f, 2.f, 2.f);
        _toolbar.setHorizontalAlign(Gui::Align::Left);
        _sceneLabel = _toolbar.addChildComponent<Gui::Label>("Scenes"_atom);
        _sceneLabel->setTextColor(Core::Color4_LightGray);
        _sceneLabel->setWidth(54.f);
        _scenePicker = _toolbar.addChildComponent<Gui::ComboModelBased>("Scenes"_atom);
        _scenePicker->setWidth(220.f);
        _scenePicker->setSizeProvider([&scenes] { return scenes.getScenes().size(); });
        _scenePicker->setDataProvider(
            [&scenes](std::size_t index, StringAtom& label) -> const void*
            {
                const auto* scene = scenes.getScenes()[index].get();
                label = scene->getSceneName();
                return scene;
            });
        _filter.setPlaceholder("Filter scenes..."_atom);
        _filter.setWidth(240.f);
        _filter.initialize();
        _scenePicker->setPopupHeaderDrawer([this] { _filter.tick(0.f); });
        _scenePicker->setItemFilter(
            [this](const StringAtom& name)
            {
                const auto& filter = _filter.getInputtedData();
                return filter.empty()
                       || ImStristr(name.c_str(), nullptr, filter.c_str(), nullptr) != nullptr;
            });
        _trashButton.setText(ICON_FA_TRASH ""_atom);
        _trashButton.setWidth(28.f);
        _trashButton.setButtonColor(Core::Color4(72, 52, 56, 255));
        _trashButton.setButtonHoverColor(Core::Color4(128, 60, 66, 255));
        _trashButton.setTextColor(Core::Color4(235, 150, 150, 255));
        _trashButton.initialize();
        _subscriptions << _trashButton.onClick->subscribeAndGetID(
            [this] { _deleteConfirmation.request(const_cast<Scene*>(_actionScene)); });
        _scenePicker->setItemActionDrawer(
            [this, &scenes](const void* data)
            {
                _actionScene = static_cast<const Scene*>(data);
                _trashButton.disableWidget(scenes.getScenes().size() == 1);
                _trashButton.tick(0.f);
                _actionScene = nullptr;
            });
        _subscriptions << _scenePicker->onSelect->subscribeAndGetID(
            [&scenes](const void* data)
            { scenes.setCurrentScene(const_cast<Scene*>(static_cast<const Scene*>(data))); });
        auto* createButton = _toolbar.addChildComponent<Gui::Button>(ICON_FA_PLUS "  New"_atom);
        createButton->setWidth(76.f);
        createButton->setButtonColor(Core::Color4(54, 86, 68, 255));
        createButton->setButtonHoverColor(Core::Color4(70, 116, 88, 255));
        createButton->setTextColor(Core::Color4(185, 235, 195, 255));
        _subscriptions << createButton->onClick->subscribeAndGetID(
            [&scenes]
            {
                auto& scene = scenes.createNewScene();
                scenes.setCurrentScene(&scene);
            });

        _toolbar.initialize();
        _deleteConfirmation.initialize(scenes);
    }

    void SceneTabs::draw(SceneManager& scenes)
    {
        drawScenePicker(scenes);
        drawTabs(scenes);
        _deleteConfirmation.draw(scenes);
    }

    void SceneTabs::drawScenePicker(SceneManager& scenes)
    {
        const auto& allScenes = scenes.getScenes();
        const auto it = std::ranges::find_if(allScenes, [&scenes](const auto& scene)
                                             { return scene.get() == scenes.getCurrentScene(); });
        _scenePicker->setCurrentIndex(static_cast<std::size_t>(it - allScenes.begin()));
        _toolbar.tick(0.f);
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
