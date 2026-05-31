/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "EditorActorAdapter.h"

#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/Misc.h"
#include "Editor/GuiComponents/VecInput.h"
#include "GameplaySystem/Entities/Actor.h"

using namespace Core::Gui;

namespace Core
{

    ECS_COMPONENT_IMPL(ECSEditorActorAdapter);

    StringAtom ECSEditorActorAdapter::getProcessedAssetType() const
    {
        return Actor::componentType;
    }

    bool ECSEditorActorAdapter::canWorkWith(BaseComponent* component) const
    {
        return dynamic_cast<Actor*>(component) != nullptr;
    }

    void ECSEditorActorAdapter::onApplyAssetData(const nlohmann::json&)
    {
        auto* comp = getTargetComponent()->tryCastTo<Actor>();
        if (!Verify(comp)) [[unlikely]]
        {
            warnLog("Can't cast component to Actor, but it must be cast!");
            return;
        }

        _actorPostDraw->input->setValue(comp->isPostDraw());
        _actorPosition->input->setInputtedData(comp->getPosition());
        _actorRotation->input->setInputtedData(comp->getRotation());
        _actorScale->input->setInputtedData(comp->getScale());
        _actorOrigin->input->setInputtedData(comp->getOrigin());
    }

    void ECSEditorActorAdapter::onInitialize()
    {
        ECSEditorMimeAdapter::onInitialize();

        constexpr float labelWidth = 120.0f;
        constexpr float inputWidth = 250.0f;

        _actorLayout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

        _actorPostDraw
            = _actorLayout.addChildComponent<LabelRow<CheckBox>>("Post draw", labelWidth);
        _subscriptionPool << _actorPostDraw->input->onChange->subscribeAndGetID(
            [this](bool value)
            {
                makeParentDirty();
                if (auto* comp = dynamic_cast<Actor*>(getTargetComponent()))
                {
                    comp->setIsPostDraw(value);
                }
            });

        _actorPosition
            = _actorLayout.addChildComponent<LabelRow<Float3Input>>("Position", labelWidth);
        _actorPosition->input->setWidth(inputWidth);
        _actorPosition->input->setFlex(Flex::Fixed);
        _subscriptionPool << _actorPosition->input->onInput->subscribeAndGetID(
            [this](const glm::vec3& value)
            {
                makeParentDirty();
                if (auto* comp = dynamic_cast<Actor*>(getTargetComponent()))
                {
                    comp->setPosition(GPos3(value));
                }
            });

        _actorRotation
            = _actorLayout.addChildComponent<LabelRow<Float3Input>>("Rotation", labelWidth);
        _actorRotation->input->setWidth(inputWidth);
        _actorRotation->input->setFlex(Flex::Fixed);
        _subscriptionPool << _actorRotation->input->onInput->subscribeAndGetID(
            [this](const glm::vec3& value)
            {
                makeParentDirty();
                if (auto* comp = dynamic_cast<Actor*>(getTargetComponent()))
                {
                    comp->setRotation(value);
                }
            });

        _actorScale = _actorLayout.addChildComponent<LabelRow<Float3Input>>("Scale", labelWidth);
        _actorScale->input->setWidth(inputWidth);
        _actorScale->input->setFlex(Flex::Fixed);
        _subscriptionPool << _actorScale->input->onInput->subscribeAndGetID(
            [this](const glm::vec3& value)
            {
                makeParentDirty();
                if (auto* comp = dynamic_cast<Actor*>(getTargetComponent()))
                {
                    comp->setScale(value);
                }
            });

        _actorOrigin = _actorLayout.addChildComponent<LabelRow<Float3Input>>("Origin", labelWidth);
        _actorOrigin->input->setWidth(inputWidth);
        _actorOrigin->input->setFlex(Flex::Fixed);
        _subscriptionPool << _actorOrigin->input->onInput->subscribeAndGetID(
            [this](const glm::vec3& value)
            {
                makeParentDirty();
                if (auto* comp = dynamic_cast<Actor*>(getTargetComponent()))
                {
                    comp->setOrigin(value);
                }
            });
    }

    void ECSEditorActorAdapter::onDraw(float dt)
    {
        if (Gui::CollapsingHeader("Actor properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _actorLayout.tick(dt);
        }
    }
} // namespace Core