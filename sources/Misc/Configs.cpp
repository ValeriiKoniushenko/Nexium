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

#include "Configs.h"

namespace Core::Config
{
    namespace Path
    {
        const std::filesystem::path projectAbsPath = NEXIUM_PROJECT_DIR;

        const std::filesystem::path gameConfigModel3DsDir = "model3ds";
        const std::filesystem::path configDir = "config";
        const std::filesystem::path editorConfigDir = "editor";

        const std::filesystem::path data = projectAbsPath / "data";
        const std::filesystem::path assets = data / "assets";
        const std::filesystem::path fonts = assets / "fonts";
        const std::filesystem::path shaders = assets / "shaders";
        const std::filesystem::path images = assets / "images";
        const std::filesystem::path objects3d = assets / "objects-3d";

        const std::filesystem::path config = data / configDir;
        const std::filesystem::path imGuiWindowsIni = config / "windows.ini";
        const std::filesystem::path editorFont = fonts / "JetBrainsMono-Regular.ttf";
        const std::filesystem::path fontAwesome = fonts / "fontawesome-webfont.ttf";
    } // namespace Path

} // namespace Core::Config