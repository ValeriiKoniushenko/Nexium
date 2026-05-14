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

#include "Core/Delegate.h"
#include "Core/Size.h"
#include "ECS/BaseComponent.h"
#include "RawDataManagement/DataStream.h"

namespace Core
{
    /**
     * BaseEditorWindowComponent or briefly BaseEWC
     */
    CLASS();
    class BaseEWC : public BaseComponent, public IDataIO
    {
        R_FRIEND_DECL(Core::BaseEWC, Core::BaseComponent);
        ECS_COMPONENT_DECL(BaseEWC, BaseComponent);

    public:
        [[nodiscard]] const StringAtom& getWindowTitle() { return getComponentName(); }

        virtual void putArguments(const StringAtom& args) {}

        [[nodiscard]] virtual const char* getIcon() { return nullptr; }

        void openWindow(const StringAtom& args = ""_atom);

        void closeWindow();

        void requestFocus() noexcept;

        [[nodiscard]] StringAtom getCacheHash() const override;
        [[nodiscard]] std::filesystem::path getCacheDir() const override;

    protected:
        virtual void onOpen() {}

        virtual void onClose() {}

        void onTick(float delta) final;

        virtual void onUpdate() {}

        virtual void onDraw() = 0;

        [[nodiscard]] virtual bool beginWindowDraw() = 0;

        virtual void endWindowDraw() = 0;

        virtual void preOpenedEndWindowDraw() {}

        [[nodiscard]] bool canBeShown() const { return _isEnabled; }

    protected:
        int /*ImGuiWindowFlags*/ _windowFlags = 0;
        bool _wasFocusRequested = false;
    };

    template<class T>
    concept IsEditorWindowComponent
        = std::derived_from<std::remove_reference_t<T>, BaseEWC> && IsComponent<T>;

    template<class T>
    concept IsEditorWindowComponentOrVoid = IsEditorWindowComponent<T> || std::is_void_v<T>;

    template<class T>
    concept IsEditorWindowComponentOrBase = IsEditorWindowComponent<T> || std::same_as<T, BaseEWC>;

    CLASS();
    class BaseFloatEWC : public BaseEWC
    {
        R_FRIEND_DECL(Core::BaseFloatEWC, Core::BaseEWC);
        ECS_COMPONENT_DECL(BaseFloatEWC, BaseEWC);

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
         * will be called while the window's size changing
         * @param FSize2 new outer(full) size
         * @param FSize2 new inner size
         */
        Delegate<void(FSize2, FSize2)>::Ptr onSizeChanged
            = Delegate<void(FSize2, FSize2)>::Create();

    protected:
        void onPreInitialize() override;
        void onInitialize() override;

        void onUpdate() override;

        [[nodiscard]] bool beginWindowDraw() override;

        void endWindowDraw() override;

    protected:
        FIELD();
        Core::FSize2 _minWindowSize = Core::FSize2(200.f, 200.f);
        FIELD();
        Core::FSize2 _size;
        FIELD();
        Core::FSize2 _innerSize;
        FIELD();
        Core::FSize2 _oldSize = Core::FSize2{ -1, -1 };
        FIELD();
        glm::vec2 _position = glm::vec2{};
        FIELD();
        glm::vec2 _innerPosition = glm::vec2{};
        FIELD();
        bool _isFitContent = false;

        bool _isFocused = false;
        bool _isHovered = false;
        std::unordered_map<int /*ImGuiStyleVar_*/, float> _styles;
    };

    CLASS();
    class BaseMenuBarEWC : public BaseEWC
    {
        R_FRIEND_DECL(Core::BaseMenuBarEWC, Core::BaseEWC);
        ECS_COMPONENT_DECL(BaseMenuBarEWC, BaseEWC);

    public:
        void onInitialize() override;

    protected:
        [[nodiscard]] bool beginWindowDraw() override;

        void endWindowDraw() override;
    };
} // namespace Core

#include "BaseWindow.generated.h" // added by the code generator. Better don't move it.
