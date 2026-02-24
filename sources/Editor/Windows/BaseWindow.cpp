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

#include "BaseWindow.h"

namespace Core
{
    ECS_COMPONENT_IMPL(BaseEWC)
    ECS_COMPONENT_IMPL(BaseFloatEWC)
    ECS_COMPONENT_IMPL(BaseMenuBarEWC)

    //
    //    ______
    //    | ___ \
    //    | |_/ /  __ _  ___   ___
    //    | ___ \ / _` |/ __| / _ \
    //    | |_/ /| (_| |\__ \|  __/
    //    \____/  \__,_||___/ \___|
    // ========================================================================

    void BaseEWC::openWindow(const StringAtom& args)
    {
        initialize();

        setEnabled(true);
        onOpen();
        requestFocus();
        if (!args.isEmpty())
        {
            putArguments(args);
        }
    }

    void BaseEWC::closeWindow()
    {
        setEnabled(false);
        onClose();
    }

    void BaseEWC::requestFocus() noexcept
    {
        _wasFocusRequested = true;
    }

    void BaseEWC::onTick(float delta)
    {
        if (beginWindowDraw())
        {
            onUpdate(); // in the future maybe will be called not every tick
            onDraw();
            preOpenedEndWindowDraw();
        }
        endWindowDraw();
    }

    StringAtom BaseEWC::getCacheHash() const
    {
        StringAtom normName;
        for (unsigned char c : getComponentName())
        {
            if (c == '_' || std::isalnum(c))
            {
                normName += c;
            }
        }

        if (normName.isEmpty())
        {
            normName = "None";
        }

        return getComponentType() + "_" + normName;
    }

    std::filesystem::path BaseEWC::getCacheDir() const
    {
        return Config::Path::editorConfigDir;
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
        const glm::vec2 size = ImGui::GetWindowSize();
        _size = FSize2{ size.x, size.y };

        _isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
        _isHovered = ImGui::IsWindowHovered(ImGuiFocusedFlags_RootAndChildWindows);

        auto pos = ImGui::GetWindowPos();
        _position = glm::vec2(pos.x, pos.y);
        pos = ImGui::GetWindowContentRegionMin();
        _innerPosition = _position + glm::vec2(pos.x, pos.y);

        if (_size != _oldSize)
        {
            const auto regionMax = ImGui::GetContentRegionMax();
            const auto regionMin = ImGui::GetWindowContentRegionMin();
            _innerSize = FSize2(regionMax.x - regionMin.x, regionMax.y - regionMin.y);
            onSizeChanged->trigger(_size, _innerSize);
            _oldSize = _size;
        }
    }

    bool BaseFloatEWC::beginWindowDraw()
    {
        ImGui::SetNextWindowSizeConstraints(glm::vec2(_minWindowSize.width, _minWindowSize.height),
                                            glm::vec2(FLT_MAX, FLT_MAX));

        if (_isFitContent)
        {
            ImGui::SetNextWindowSize(glm::vec2(0, 0), ImGuiCond_Always);
        }

        for (auto [style, val] : _styles)
        {
            ImGui::PushStyleVar(style, val);
        }

        bool prevEnabledState = _isEnabled;
        const auto res = ImGui::Begin(getComponentName().c_str(), &_isEnabled, _windowFlags);
        if (_wasFocusRequested)
        {
            ImGui::SetWindowFocus(getComponentName().c_str());
            _wasFocusRequested = false;
        }
        if (prevEnabledState != _isEnabled)
        {
            if (!_isEnabled)
            {
                // emulate closing logic
                closeWindow();
            }
        }

        ImGui::PopStyleVar(static_cast<int>(_styles.size()));

        return res;
    }

    void BaseFloatEWC::endWindowDraw()
    {
        ImGui::End();
    }

    void BaseFloatEWC::onPreInitialize()
    {
        BaseEWC::onPreInitialize();
    }

    void BaseFloatEWC::onInitialize()
    {
        BaseComponent::onInitialize();

        tryReadFromCache();
    }

    //
    // ______                    ___  ___                     ______
    // | ___ \                   |  \/  |                     | ___ \
    // | |_/ /  __ _  ___   ___  | .  . |  ___  _ __   _   _  | |_/ /  __ _  _ __
    // | ___ \ / _` |/ __| / _ \ | |\/| | / _ \| '_ \ | | | | | ___ \ / _` || '__|
    // | |_/ /| (_| |\__ \|  __/ | |  | ||  __/| | | || |_| | | |_/ /| (_| || |
    // \____/  \__,_||___/ \___| \_|  |_/ \___||_| |_| \__,_| \____/  \__,_||_|
    // ========================================================================
    void BaseMenuBarEWC::onInitialize()
    {
        BaseEWC::onInitialize();
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