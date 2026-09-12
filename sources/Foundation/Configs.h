// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Color.h"
#include "Core/Size.h"

#include <filesystem>

namespace Foundation::Config
{
    // Default color pallet
    constexpr Core::Color4 ColorRed = Core::Color4(255, 25, 25, 255);
    constexpr Core::Color4 ColorGreen = Core::Color4(25, 255, 25, 255);
    constexpr Core::Color4 ColorSoftGreen = Core::Color4(25, 204, 25, 204);
    constexpr Core::Color4 ColorGrey = Core::Color4(153, 153, 153, 255);
    constexpr Core::Color4 ColorSoftWhite = Core::Color4(190, 190, 190, 255);
    constexpr Core::Color4 ColorWhite = Core::Color4(234, 237, 239, 255);
    constexpr Core::Color4 ColorYellow = Core::Color4(255, 255, 25, 255);
    constexpr Core::Color4 ColorHalfYellow = Core::Color4(204, 102, 25, 255);
    constexpr Core::Color4 ColorBlue = Core::Color4(25, 25, 255, 255);

    namespace Path
    {
        extern const std::filesystem::path projectAbsPath;

        extern const std::filesystem::path gameConfigModel3DsDir;
        extern const std::filesystem::path configDir;
        extern const std::filesystem::path editorConfigDir;
        extern const std::filesystem::path cacheDir;

        extern const std::filesystem::path data;
        extern const std::filesystem::path assets;
        extern const std::filesystem::path fonts;
        extern const std::filesystem::path shaders;
        extern const std::filesystem::path images;
        extern const std::filesystem::path objects3d;

        extern const std::filesystem::path config;
        extern const std::filesystem::path imGuiWindowsIni;
        extern const std::filesystem::path editorFont;
        extern const std::filesystem::path fontAwesome;
    } // namespace Path

    // TODO: move to editor's config
    static constexpr float defaultEditorFontSize = 16.f;
    static constexpr float defaultEmojiScale = 0.8f;

    inline static const char* spdlogPattern = "%D [%L] [%n] %v";
    inline static const char* defaultWindowName = "Nexium Engine";
    static constexpr Core::ISize2 defaultWindowSize = Core::ISize2{ 1200, 800 };

} // namespace Foundation::Config
