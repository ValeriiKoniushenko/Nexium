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

#include "BaseWindow.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(BaseEWC)
    ECS_REGISTER_NEW_COMPONENT_TYPE(BaseFloatEWC)
    ECS_REGISTER_NEW_COMPONENT_TYPE(BaseMenuBarEWC)

    //
    //    ______
    //    | ___ \
    //    | |_/ /  __ _  ___   ___
    //    | ___ \ / _` |/ __| / _ \
    //    | |_/ /| (_| |\__ \|  __/
    //    \____/  \__,_||___/ \___|
    // ========================================================================

    void BaseEWC::onTick()
    {
        if (beginWindowDraw())
        {
            onUpdate(); // in the future maybe will be called not every tick
            onDraw();
        }
        endWindowDraw();
    }

    //
    //    ______                   ______  _                _
    //    | ___ \                  |  ___|| |              | |
    //    | |_/ /  __ _  ___   ___ | |_   | |  ___    __ _ | |_
    //    | ___ \ / _` |/ __| / _ \|  _|  | | / _ \  / _` || __|
    //    | |_/ /| (_| |\__ \|  __/| |    | || (_) || (_| || |_
    //    \____/  \__,_||___/ \___|\_|    |_| \___/  \__,_| \__|
    // ========================================================================
    void BaseFloatEWC::setFitContent(bool v)
    {
        _isFitContent = v;
        if (v)
        {
            _windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
        }
        else
        {
            _windowFlags &= ~ImGuiWindowFlags_AlwaysAutoResize;
        }
    }

    void BaseFloatEWC::onUpdate()
    {
        const ImVec2 size = ImGui::GetWindowSize();
        _size = FSize2{ size.x, size.y };

        if (_size != _oldSize)
        {
            const auto regionMax = ImGui::GetContentRegionMax();
            const auto regionMin = ImGui::GetWindowContentRegionMin();
            _innerSize = FSize2(regionMax.x - regionMin.x, regionMax.y - regionMin.y);
            onSizeChanged.trigger(_size, _innerSize);
            _oldSize = _size;
        }
    }

    bool BaseFloatEWC::beginWindowDraw()
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(300, 200), ImVec2(FLT_MAX, FLT_MAX));

        if (_isFitContent)
        {
            ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
        }

        for (auto [style, val] : _styles)
        {
            ImGui::PushStyleVar(style, val);
        }

        const auto res = ImGui::Begin(getComponentName().c_str(), &_isEnabled, _windowFlags);

        ImGui::PopStyleVar(static_cast<int>(_styles.size()));

        return res;
    }

    void BaseFloatEWC::endWindowDraw()
    {
        ImGui::End();
    }

    void BaseFloatEWC::onInit()
    {
        AbstractComponent::onInit();
    }

    //
    // ______                    ___  ___                     ______
    // | ___ \                   |  \/  |                     | ___ \
    // | |_/ /  __ _  ___   ___  | .  . |  ___  _ __   _   _  | |_/ /  __ _  _ __
    // | ___ \ / _` |/ __| / _ \ | |\/| | / _ \| '_ \ | | | | | ___ \ / _` || '__|
    // | |_/ /| (_| |\__ \|  __/ | |  | ||  __/| | | || |_| | | |_/ /| (_| || |
    // \____/  \__,_||___/ \___| \_|  |_/ \___||_| |_| \__,_| \____/  \__,_||_|
    // ========================================================================
    void BaseMenuBarEWC::onInit()
    {
        BaseEWC::onInit();
    }

    bool BaseMenuBarEWC::beginWindowDraw()
    {
        return ImGui::BeginMainMenuBar();
    }

    void BaseMenuBarEWC::endWindowDraw()
    {
        ImGui::EndMainMenuBar();
    }

} // namespace Core
