// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "BaseWindow.h"

#include "Foundation/Configs.h"
#include "ImGui/imgui.h"

namespace Core
{
    ECS_IMPL(BaseEWC);
    ECS_IMPL(BaseFloatEWC);
    ECS_IMPL(BaseMenuBarEWC);

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
        if (_windowTitle.isEmpty())
        {
            _windowTitle = getComponentName();
        }

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
        const auto res = ImGui::Begin(_windowTitle.c_str(), &_isEnabled, _windowFlags);
        if (_wasFocusRequested)
        {
            ImGui::SetWindowFocus(_windowTitle.c_str());
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

        GetCacheSystem().tryRead(*this);
    }

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
