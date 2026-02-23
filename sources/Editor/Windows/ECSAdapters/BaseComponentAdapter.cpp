/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "BaseComponentAdapter.h"


#include "BaseComponentAdapter.generated.cpp.inl" // this line added by the code generator.

using namespace Core::Gui;

namespace Core
{

    ECS_COMPONENT_IMPL(ECSBaseComponentAdapter);

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