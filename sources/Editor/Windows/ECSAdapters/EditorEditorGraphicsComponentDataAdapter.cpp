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

#include "EditorGraphicsComponentDataAdapter.h"
#include "Graphics/GraphicsComponents.h"

using namespace Core::Gui;

namespace Core
{

    ECS_COMPONENT_IMPL(ECSEditorGraphicsComponentDataAdapter);

    bool ECSEditorGraphicsComponentDataAdapter::canWorkWith(BaseComponent* component) const
    {
        return dynamic_cast<GraphicsComponentData*>(component) != nullptr;
    }

    void ECSEditorGraphicsComponentDataAdapter::onApplyAssetData(const nlohmann::json&)
    {
        auto* comp = dynamic_cast<GraphicsComponentData*>(getTargetComponent());
        if (!Verify(comp)) [[unlikely]]
        {
            warnLog("Can't cast component to Actor, but it must be cast!");
            return;
        }
    }

    void ECSEditorGraphicsComponentDataAdapter::onInitialize()
    {
        ECSEditorMimeAdapter::onInitialize();

        constexpr float labelWidth = 120.0f;
    }

    void ECSEditorGraphicsComponentDataAdapter::onDraw(float dt)
    {
        if (Gui::CollapsingHeader("GraphicsComponentData properties",
                                  ImGuiTreeNodeFlags_DefaultOpen))
        {
            _layout.tick(dt);
        }
    }
} // namespace Core