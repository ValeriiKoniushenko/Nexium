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

#include "Core/Color.h"
#include "Core/Size.h"
#include "ImGui/imgui.h"
#include "InputDevices/Keyboard.h"

#include <filesystem>

namespace Core::Config
{
    // Default color pallet
    static constexpr auto ColorRed = Color4(255, 25, 25, 255);
    static constexpr auto ColorGreen = Color4(25, 255, 25, 255);
    static constexpr auto ColorSoftGreen = Color4(25, 204, 25, 204);
    static constexpr auto ColorGrey = Color4(153, 153, 153, 255);
    static constexpr auto ColorSoftWhite = Color4(190, 190, 190, 255);
    static constexpr auto ColorWhite = Color4(234, 237, 239, 255);
    static constexpr auto ColorYellow = Color4(255, 255, 25, 255);
    static constexpr auto ColorHalfYellow = Color4(204, 102, 25, 255);
    static constexpr auto ColorBlue = Color4(25, 25, 255, 255);

    namespace Path
    {
        inline static std::filesystem::path gameConfigModel3DsDir = "model3ds";
        inline static std::filesystem::path configDir = "config";
        inline static std::filesystem::path editorConfigDir = "editor";
        inline static std::filesystem::path projectAbsPath = NEXIUM_PROJECT_DIR;
        inline static std::filesystem::path assets = projectAbsPath / "assets";
        inline static std::filesystem::path fonts = assets / "fonts";
        inline static std::filesystem::path shaders = assets / "shaders";
        inline static std::filesystem::path images = assets / "images";
        inline static std::filesystem::path objects3d = assets / "objects-3d";
        inline static std::filesystem::path imGuiWindowsIni
            = projectAbsPath / configDir / "windows.ini";

        inline static std::filesystem::path editorFont = fonts / "JetBrainsMono-Regular.ttf";
        inline static std::filesystem::path fontAwesome = fonts / "fontawesome-webfont.ttf";
    } // namespace Path

    namespace Keyboard
    {
        inline static auto editorImGuiShowRect = Core::Keyboard::Key::Key_F2;
    }

    static constexpr float windowAspectRatio = 16.f / 9.f;
    static constexpr float defaultEditorFontSize = 16.f;
    static constexpr float defaultEmojiScale = 0.8f;

    static constexpr int defaultEditorImGuiFlags
        = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;

    inline static const char* spdlogPattern = "%D [%L] [%n] %v";
    inline static const char* defaultWindowName = "Nexium Engine";
    static constexpr ISize2 defaultWindowSize = ISize2{ 1200, 800 };
} // namespace Core::Config