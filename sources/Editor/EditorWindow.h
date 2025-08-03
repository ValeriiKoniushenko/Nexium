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
#include "Core/String.h"
#include "GameplaySystem/ECS/BaseComponent.h"
#include "ImGui/imgui.h"

namespace SW
{
    class BaseEditorWindowComponent : public BaseComponent
    {
        ECS_REGISTER_NEW_COMPONENT(BaseEditorWindowComponent, BaseComponent);

    public:
        [[nodiscard]] const Core::StringAtom& getWindowTitle() { return _windowTitle; }

    protected:
        void onTick() override final;
        virtual void onUpdate() {};
        virtual void onDraw() = 0;

        [[nodiscard]] virtual bool beginWindowDraw() = 0;
        virtual void endWindowDraw() = 0;

    protected:
        ImGuiWindowFlags _windowFlags = 0;
        Core::StringAtom _windowTitle = "Window";
        bool _pOpen = false;
    };

    class BaseFloatEditorWindowComponent : public BaseEditorWindowComponent
    {
        ECS_REGISTER_NEW_COMPONENT(BaseFloatEditorWindowComponent, BaseEditorWindowComponent);

    public:
        [[nodiscard]] Core::FSize2 getWindowSize() const noexcept { return _size; }

    protected:
        void onUpdate() override;
        [[nodiscard]] bool beginWindowDraw() override;
        void endWindowDraw() override;

    protected:
        Core::FSize2 _size;
    };

    template<class T>
    concept IsEditorWindowComponent
        = std::derived_from<T, BaseEditorWindowComponent> && IsComponent<T>;

    class BaseMenuBarWindowComponent : public BaseEditorWindowComponent
    {
        ECS_REGISTER_NEW_COMPONENT(BaseMenuBarWindowComponent, BaseEditorWindowComponent);

    public:
        void onInit() override;

    protected:
        [[nodiscard]] bool beginWindowDraw() override;
        void endWindowDraw() override;
    };

    class GameViewportWindow : public BaseFloatEditorWindowComponent
    {
        ECS_REGISTER_NEW_COMPONENT(GameViewportWindow, BaseFloatEditorWindowComponent);

    public:
    protected:
        void onInit() override;
        void onDraw() override;
    };

    class KeyboardShortcutsTipsWindow : public BaseFloatEditorWindowComponent
    {
        ECS_REGISTER_NEW_COMPONENT(KeyboardShortcutsTipsWindow, BaseFloatEditorWindowComponent);

    public:
    private:
        void onInit() override;
        void onDraw() override;
    };

} // namespace SW