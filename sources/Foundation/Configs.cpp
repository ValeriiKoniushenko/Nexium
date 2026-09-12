// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Configs.h"

namespace Foundation::Config
{
    namespace Path
    {
        const std::filesystem::path projectAbsPath = NEXIUM_PROJECT_DIR;

        const std::filesystem::path gameConfigModel3DsDir = "model3ds";
        const std::filesystem::path configDir = "config";
        const std::filesystem::path editorConfigDir = "editor";
        const std::filesystem::path cacheDir = "cache";

        const std::filesystem::path data = projectAbsPath / "data";
        const std::filesystem::path assets = data / "assets";
        const std::filesystem::path fonts = assets / "fonts";
        const std::filesystem::path shaders = assets / "shaders";
        const std::filesystem::path images = assets / "images";
        const std::filesystem::path objects3d = assets / "objects-3d";

        const std::filesystem::path config = data / configDir;
        const std::filesystem::path cache = data / cacheDir;
        const std::filesystem::path imGuiWindowsIni = cache / "windows.ini";
        const std::filesystem::path editorFont = fonts / "JetBrainsMono-Regular.ttf";
        const std::filesystem::path fontAwesome = fonts / "fontawesome-webfont.ttf";
    } // namespace Path

} // namespace Foundation::Config
