// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "Core/Color.h"
#include "GameplaySystem/ECS/BaseComponent.h"
#include "ImGui/imgui.h"

namespace Core
{

    class Widget : public BaseComponent
    {
        ECS_REGISTER_NEW_COMPONENT(Widget, BaseComponent);

    public:
    protected:
        void onTick(float delta) override;
        virtual void onDraw() = 0;
    };

    [[nodiscard]] ImVec4 colorToImVec4(const Color4& color);

} // namespace Core

namespace ImGui
{
    void PushStyleColor(ImGuiCol idx, const Core::Color4& col);
    bool OptPushStyleColor(ImGuiCol idx, const std::optional<Core::Color4>& col);
    bool OptPushStyleVar(ImGuiStyleVar idx, const std::optional<float>& col);
} // namespace ImGui