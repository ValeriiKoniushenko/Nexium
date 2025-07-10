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

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "Core/Delegate.h"
#include "Core/Size.h"
#include "ModuleInfo.h"

#include <Core/Singleton.h>
#include <filesystem>

namespace SW
{

    class Window : public Core::StrictSingleton<Window>, public BaseLog
    {
    public:
        Window() = default;
        ~Window() override;

        void create(const Core::StringAtom& title, Core::ISize2 size = { 300, 300 });
        void clear(int code);

        [[nodiscard]] bool shouldClose() const;

        void swapBuffers();

        void pollEvent();

        void viewport(GLint x, GLint y, GLsizei width, GLsizei height);

        [[nodiscard]] Core::ISize2 getSize() const;

        void setCursorPosition(double x, double y);

        Core::Delegate<void(int, int, int, int)> onKeyPressed;
        Core::Delegate<void(unsigned int)> onTextInput;
        Core::Delegate<void(int)> onCursorEntered;
        Core::Delegate<void(double, double)> onMouseWheel;

        [[nodiscard]] spdlog::logger* getLogger() const override
        {
            return RawGraphics::getLogger();
        }
        [[nodiscard]] const char* getPrefix() const override { return "Window"; }

    protected:
        GLFWwindow* _window{};
        Core::ISize2 _size{};
        Core::StringAtom _title;
    };

    Window& GetWindow();

} // namespace SW