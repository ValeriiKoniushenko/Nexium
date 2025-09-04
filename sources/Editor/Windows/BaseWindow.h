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

#include "Core/Delegate.h"
#include "GameplaySystem/ECS/BaseComponent.h"
#include "Misc/ImGuiHelper.h"

namespace Core
{
    /**
     * @brief BaseEditorWindowComponent or briefly BaseEWC
     */
    class BaseEWC : public BaseComponent
    {
        ECS_REGISTER_NEW_COMPONENT(BaseEWC, BaseComponent);

    public:
        static constexpr ImVec4 ColorRed = ImVec4(1.0f, 0.1f, 0.1f, 1.0f);
        static constexpr ImVec4 ColorGreen = ImVec4(0.1f, 1.0f, 0.1f, 1.0f);
        static constexpr ImVec4 ColorSoftGreen = ImVec4(0.1f, 0.8f, 0.1f, 0.8f);
        static constexpr ImVec4 ColorGrey = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        static constexpr ImVec4 ColorYellow = ImVec4(1.f, 1.0f, 0.1f, 1.0f);
        static constexpr ImVec4 ColorHalfYellow = ImVec4(0.8f, 0.4f, 0.1f, 1.0f);
        static constexpr ImVec4 ColorBlue = ImVec4(0.1f, 0.1f, 1.0f, 1.0f);

    public:
        [[nodiscard]] const StringAtom& getWindowTitle() { return getComponentName(); }
        virtual void putArguments(const StringAtom& args) {}

    protected:
        void onTick(float delta) final;
        virtual void onUpdate() {};
        virtual void onDraw() = 0;

        [[nodiscard]] virtual bool beginWindowDraw() = 0;
        virtual void endWindowDraw() = 0;

        [[nodiscard]] bool canBeShown() const { return _isEnabled; }

    protected:
        ImGuiWindowFlags _windowFlags = 0;
    };

    template<class T>
    concept IsEditorWindowComponent = std::derived_from<T, BaseEWC> && IsComponent<T>;

    template<class T>
    concept IsEditorWindowComponentOrVoid = IsEditorWindowComponent<T> || std::is_void_v<T>;

    template<class T>
    concept IsEditorWindowComponentOrBase = IsEditorWindowComponent<T> || std::same_as<T, BaseEWC>;

    class BaseFloatEWC : public BaseEWC
    {
        ECS_REGISTER_NEW_COMPONENT(BaseFloatEWC, BaseEWC);

    public:
        [[nodiscard]] FSize2 getWindowSize() const noexcept { return _size; }
        [[nodiscard]] FSize2 getInnerWindowSize() const noexcept { return _innerSize; }

        void setFitContent(bool v);
        [[nodiscard]] bool isFitContent() const noexcept { return _isFitContent; }

        [[nodiscard]] bool isFocused() const noexcept { return _isFocused; }
        [[nodiscard]] bool isHovered() const noexcept { return _isHovered; }

        [[nodiscard]] glm::vec2 getPosition() const noexcept { return _position; }
        [[nodiscard]] glm::vec2 getInnerPosition() const noexcept { return _innerPosition; }

        /**
         * @brief will be called while the window's size changing
         * @param FSize2 new outer(full) size
         * @param FSize2 new inner size
         */
        Delegate<void(FSize2, FSize2)> onSizeChanged;

    protected:
        void onInitialize() override;
        void onUpdate() override;
        [[nodiscard]] bool beginWindowDraw() override;
        void endWindowDraw() override;

    protected:
        FSize2 _minWindowSize = FSize2(200.f, 20.f);
        FSize2 _size;
        FSize2 _innerSize;
        FSize2 _oldSize = FSize2{ -1, -1 };
        glm::vec2 _position;
        glm::vec2 _innerPosition;
        bool _isFitContent = false;
        bool _isFocused = false;
        bool _isHovered = false;
        std::unordered_map<ImGuiStyleVar_, float> _styles;
    };

    class BaseMenuBarEWC : public BaseEWC
    {
        ECS_REGISTER_NEW_COMPONENT(BaseMenuBarEWC, BaseEWC);

    public:
        void onInitialize() override;

    protected:
        [[nodiscard]] bool beginWindowDraw() override;
        void endWindowDraw() override;
    };
} // namespace Core
