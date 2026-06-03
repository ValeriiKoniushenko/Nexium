/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "../ResourceManagement/DataStream.h"

#include <functional>

namespace Core
{
    class StaticMesh;
    class Actor;
    class BaseCamera;

    CLASS();
    class IDrawable : public IDataIO
    {
        R_FRIEND(IDrawable);

    public:
        IDrawable(const IDrawable&) = default;
        IDrawable& operator=(const IDrawable&) = default;
        IDrawable(IDrawable&&) noexcept = default;
        IDrawable& operator=(IDrawable&&) noexcept = default;
        ~IDrawable() override = default;

        /**
         * In the best world, you shouldn't call this function directly; another class does a
         * drawing process. But if you really need it: it will draw an object with the default
         * shader. Single draw bundle!
         */
        virtual void draw(BaseCamera&) {}

        /**
         * In the best world, you shouldn't call this function directly; another class does a
         * drawing process. But if you really need it:
         * It makes the minimal draw only with graphics modifiers - nothing more.
         * Before using of this function you must manually prepare the shader &
         * 'use' it.
         */
        virtual void pureDraw(const std::function<void(StaticMesh*)>& onUniformSet,
                              const std::function<bool(const Actor*)>& conditional)
        {
        }

        void setIsPostDraw(bool value) noexcept { _isPostDraw = value; }
        [[nodiscard]] bool isPostDraw() const noexcept { return _isPostDraw; }

        // override -> IDataIO
        [[nodiscard]] StringAtom getCacheHash() const override { return "IDrawable"_atom; }

    protected:
        IDrawable() = default;

    private:
        FIELD();
        bool _isPostDraw = false;
    };
} // namespace Core

#include "IDrawable.generated.h" // added by the code generator. Better don't move it.
