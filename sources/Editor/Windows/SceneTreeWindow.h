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

#pragma once

#include "BaseWindow.h"

namespace Core
{
    class Scene;
    class Actor;

    class SceneTreeWindowEWC : public BaseFloatEWC
    {
        ECS_COMPONENT_DECL(SceneTreeWindowEWC, BaseFloatEWC);

    public:
        void setScene(Scene* scene) { _scene = scene; }
        [[nodiscard]] Scene* getScene() const noexcept { return _scene; }

        [[nodiscard]] const char* getIcon() override { return ICON_FA_GLOBE; }

    public:
        BaseComponent* selectedObject = nullptr;

    protected:
        void onInitialize() override;

        void onDraw() override;

        void onUpdate() override;

    protected:
        Scene* _scene = nullptr;
        int _commonTreeFlags
            = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        BaseComponent* _lastSelectedObject = nullptr;

    private:
        bool drawTreeNode(BaseComponent* n, int32_t id, bool isInSelectedSubtree = false);
    };
} // namespace Core