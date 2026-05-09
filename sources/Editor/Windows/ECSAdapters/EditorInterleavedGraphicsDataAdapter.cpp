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

#include "EditorInterleavedGraphicsDataAdapter.h"

#include "Editor/GuiComponents/Misc.h"
#include "Graphics/GraphicsComponents.h"

using namespace Core::Gui;

namespace Core
{

    ECS_COMPONENT_IMPL(ECSEditorInterleavedGraphicsDataAdapter);

    bool ECSEditorInterleavedGraphicsDataAdapter::canWorkWith(BaseComponent* component) const
    {
        return dynamic_cast<InterleavedGraphicsData*>(component) != nullptr;
    }

    void ECSEditorInterleavedGraphicsDataAdapter::onApplyAssetData(const nlohmann::json& json)
    {
        auto* comp = dynamic_cast<InterleavedGraphicsData*>(getTargetComponent());
        if (!Verify(comp)) [[unlikely]]
        {
            warnLog("Can't cast component to InterleavedGraphicsData, but it must be cast!");
            return;
        }

        _modifiers->input->clearData(true);
        for (auto&& modifier : comp->getDrawModifiers())
        {
            _modifiers->input->add(modifier, true);
        }
    }

    void ECSEditorInterleavedGraphicsDataAdapter::onInitialize()
    {
        ECSEditorMimeAdapter::onInitialize();

        constexpr float labelWidth = 120.0f;

        _layout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

        _modifiers
            = _layout.addChildComponent<LabelRow<GraphicsModifiersArray>>("Modifiers", labelWidth);
        _modifiers->setVerticalAlign(Align::Top);
        _subscriptionPool << _modifiers->input->onChange->subscribeAndGetID([this]()
                                                                            { makeParentDirty(); });
        _subscriptionPool << _modifiers->input->onSave->subscribeAndGetID(
            [this](auto&& params)
            {
                auto* comp = dynamic_cast<InterleavedGraphicsData*>(getTargetComponent());
                if (!Verify(comp)) [[unlikely]]
                {
                    warnLog(
                        "Can't cast component to InterleavedGraphicsData, but it must be cast!");
                    return;
                }
                comp->setDrawModifiers(params);
            });
    }

    void ECSEditorInterleavedGraphicsDataAdapter::onDraw(float dt)
    {
        if (Gui::CollapsingHeader("InterleavedGraphicsData properties",
                                  ImGuiTreeNodeFlags_DefaultOpen))
        {
            _layout.tick(dt);
        }
    }
} // namespace Core