// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "BaseComponentAdapter.h"

#include "Editor/GuiComponents/Array.h"
#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/Misc.h"

using namespace Core::Gui;

namespace Core
{

    ECS_IMPL(ECSBaseComponentAdapter);

    void ECSBaseComponentAdapter::onApplyAssetData(const nlohmann::json& json)
    {
        auto* comp = getTargetComponent();
        _ecsName->input->setInputtedData(comp->getComponentName().toStdString());
        _ecsType->input->setInputtedData(comp->getComponentType().toStdString());
        if (comp->hasParent())
        {
            _ecsParent->input->setInputtedData(comp->getParent()->getComponentName().toStdString());
        }
        else
        {
            _ecsParent->input->setInputtedData("");
        }

        _ecsDisableTicks->input->setValue(comp->getNoTick());

        _ecsEnabledComponent->input->setValue(comp->isEnabled());

        _ecsChildren->input->clearData();
        for (auto&& child : comp->getChildren())
        {
            _ecsChildren->input->add("{} [{}]"_f << child->getComponentName()
                                                 << child->getComponentType());
        }
    }

    void ECSBaseComponentAdapter::onInitialize()
    {
        ECSEditorMimeAdapter::onInitialize();

        constexpr float labelWidth = 120.0f;

        _baseEcsLayout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

        _ecsEnabledComponent
            = _baseEcsLayout.addChildComponent<LabelRow<CheckBox>>("Enabled", labelWidth);
        _subscriptionPool << _ecsEnabledComponent->input->onChange->subscribeAndGetID(
            [this](bool value)
            {
                makeParentDirty();
                if (auto* comp = getTargetComponent())
                {
                    comp->setEnabled(value);
                }
            });

        _ecsName = _baseEcsLayout.addChildComponent<LabelRow<TextInput>>("Name", labelWidth);
        _ecsName->input->setFlex(Flex::FlexWidth);
        _subscriptionPool << _ecsName->input->onInput->subscribeAndGetID(
            [this](const char* value)
            {
                makeParentDirty();
                if (auto* comp = getTargetComponent())
                {
                    comp->setComponentName(value);
                }
            });

        _ecsType = _baseEcsLayout.addChildComponent<LabelRow<TextInput>>("Type", labelWidth);
        _ecsType->input->setFlex(Flex::FlexWidth);
        _ecsType->input->setReadOnly(true);

        _ecsParent = _baseEcsLayout.addChildComponent<LabelRow<TextInput>>("Parent", labelWidth);
        _ecsParent->input->setFlex(Flex::FlexWidth);
        _ecsParent->input->setReadOnly(true);

        _ecsDisableTicks
            = _baseEcsLayout.addChildComponent<LabelRow<CheckBox>>("No ticks", labelWidth);
        _subscriptionPool << _ecsDisableTicks->input->onChange->subscribeAndGetID(
            [this](bool value)
            {
                makeParentDirty();
                if (auto* comp = getTargetComponent())
                {
                    comp->setNoTick(value);
                }
            });

        _ecsChildren
            = _baseEcsLayout.addChildComponent<LabelRow<StringArray>>("Children", labelWidth);
        _ecsChildren->setVerticalAlign(Align::Top);
        _ecsChildren->input->setReadOnly(true);
    }

    void ECSBaseComponentAdapter::onDraw(float dt)
    {
        if (Gui::CollapsingHeader("Base ECS properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _baseEcsLayout.tick(dt);
        }
    }
} // namespace Core