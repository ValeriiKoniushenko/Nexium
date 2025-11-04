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

#include "EditorActorAdapter.h"

using namespace Core::Gui;

namespace Core
{

    ECS_COMPONENT_IMPL(ECSEditorActorAdapter);

    void ECSEditorActorAdapter::onApplyAssetData(const nlohmann::json& json)
    {
        if (json.contains("isPostDraw"))
        {
            _actorPostDraw->input->setValue(json["isPostDraw"].get<bool>());
        }
        if (json.contains("position"))
        {
            _actorPosition->input->setInputtedData(json["position"].get<glm::vec3>());
        }
        if (json.contains("rotation"))
        {
            _actorRotation->input->setInputtedData(json["rotation"].get<glm::vec3>());
        }
        if (json.contains("scale"))
        {
            _actorScale->input->setInputtedData(json["scale"].get<glm::vec3>());
        }
        if (json.contains("origin"))
        {
            _actorOrigin->input->setInputtedData(json["origin"].get<glm::vec3>());
        }
    }

    void ECSEditorActorAdapter::onInitialize()
    {
        ECSEditorMimeAdapter::onInitialize();

        constexpr float labelWidth = 120.0f;
        constexpr float inputWidth = 250.0f;

        _actorLayout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

        _actorPostDraw
            = _actorLayout.addChildComponent<LabelRow<CheckBox>>("Post draw", labelWidth);
        _actorPostDraw->input->onChange.subscribe([this](auto) { makeParentDirty(); });

        _actorPosition
            = _actorLayout.addChildComponent<LabelRow<Float3Input>>("Position", labelWidth);
        _actorPosition->input->setWidth(inputWidth);
        _actorPosition->input->setFlex(Flex::Fixed);
        _actorPosition->input->onInput.subscribe([this](auto) { makeParentDirty(); });

        _actorRotation
            = _actorLayout.addChildComponent<LabelRow<Float3Input>>("Rotation", labelWidth);
        _actorRotation->input->setWidth(inputWidth);
        _actorRotation->input->setFlex(Flex::Fixed);
        _actorRotation->input->onInput.subscribe([this](auto) { makeParentDirty(); });

        _actorScale = _actorLayout.addChildComponent<LabelRow<Float3Input>>("Scale", labelWidth);
        _actorScale->input->setWidth(inputWidth);
        _actorScale->input->setFlex(Flex::Fixed);
        _actorScale->input->onInput.subscribe([this](auto) { makeParentDirty(); });

        _actorOrigin = _actorLayout.addChildComponent<LabelRow<Float3Input>>("Origin", labelWidth);
        _actorOrigin->input->setWidth(inputWidth);
        _actorOrigin->input->setFlex(Flex::Fixed);
        _actorOrigin->input->onInput.subscribe([this](auto) { makeParentDirty(); });
    }

    void ECSEditorActorAdapter::onDraw(float dt)
    {
        if (Gui::CollapsingHeader("Actor properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _actorLayout.tick(dt);
        }
    }
} // namespace Core