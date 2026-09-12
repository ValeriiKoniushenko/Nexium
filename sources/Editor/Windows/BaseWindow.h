// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Delegate.h"
#include "Core/Size.h"
#include "Foundation/Interfaces/DataStream.h"
#include "NxFundamental/ECS/BaseComponent.h"

namespace Core
{
    /// BaseEditorWindowComponent or briefly BaseEWC
    CLASS();
    class BaseEWC : public BaseComponent, public Foundation::IDataIO
    {
        ECS_DECL(BaseEWC, Core::BaseComponent);

    public:
        [[nodiscard]] const StringAtom& getWindowTitle() const
        {
            return _windowTitle.isEmpty() ? getComponentName() : _windowTitle;
        }

        virtual void putArguments(const StringAtom& args) {}

        [[nodiscard]] virtual const char* getIcon() { return nullptr; }

        void openWindow(const StringAtom& args = ""_atom);

        void closeWindow();

        void requestFocus() noexcept;

        [[nodiscard]] StringAtom getCacheHash() const override;

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
        StringAtom _windowTitle;
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
        ECS_DECL(BaseFloatEWC, Core::BaseEWC);

    public:
        [[nodiscard]] FSize2 getWindowSize() const noexcept { return _size; }
        [[nodiscard]] FSize2 getInnerWindowSize() const noexcept { return _innerSize; }

        void setFitContent(bool v);

        [[nodiscard]] bool isFitContent() const noexcept { return _isFitContent; }

        [[nodiscard]] bool isFocused() const noexcept { return _isFocused; }
        [[nodiscard]] bool isHovered() const noexcept { return _isHovered; }

        [[nodiscard]] glm::vec2 getPosition() const noexcept { return _position; }
        [[nodiscard]] glm::vec2 getInnerPosition() const noexcept { return _innerPosition; }

        /// will be called while the window's size changing
        /// @param FSize2 new outer(full) size
        /// @param FSize2 new inner size
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
        ECS_DECL(BaseMenuBarEWC, Core::BaseEWC);

    protected:
        void onInitialize() override;

    protected:
        [[nodiscard]] bool beginWindowDraw() override;

        void endWindowDraw() override;
    };
} // namespace Core

#include "BaseWindow.generated.h" // added by the code generator. Better don't move it.
