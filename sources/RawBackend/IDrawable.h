// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "JustReflectMe/Adapter.h"

#include <functional>

namespace RawBackend
{
    class StaticMesh;
    class Actor;
    class BaseCamera;

    CLASS();
    class IDrawable
    {
        R_FRIEND(IDrawable);

    public:
        IDrawable(const IDrawable&) = default;
        IDrawable& operator=(const IDrawable&) = default;
        IDrawable(IDrawable&&) noexcept = default;
        IDrawable& operator=(IDrawable&&) noexcept = default;
        virtual ~IDrawable() = default;

        /// In the best world, you shouldn't call this function directly; another class does a
        /// drawing process. But if you really need it: it will draw an object with the default
        /// shader. Single draw bundle!
        virtual void draw(BaseCamera&) {}

        /// In the best world, you shouldn't call this function directly; another class does a
        /// drawing process. But if you really need it:
        /// It makes the minimal draw only with graphics modifiers - nothing more.
        /// Before using of this function you must manually prepare the shader &
        /// 'use' it.
        virtual void pureDraw(const std::function<void(StaticMesh*)>& onUniformSet,
                              const std::function<bool(const Actor*)>& conditional)
        {
        }

        void setIsPostDraw(bool value) noexcept { _isPostDraw = value; }
        [[nodiscard]] bool isPostDraw() const noexcept { return _isPostDraw; }

    protected:
        IDrawable() = default;

    private:
        FIELD();
        bool _isPostDraw = false;
    };
} // namespace RawBackend

#include "IDrawable.generated.h" // added by the code generator. Better don't move it.
