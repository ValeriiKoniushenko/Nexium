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

#include "ImGui/imgui.h"

#include <filesystem>
#include <functional>
#include <string>

namespace Core
{

    struct ThumbnailActions
    {
        std::function<void()> removeSelected;
        std::function<void(const std::filesystem::path&)> cut;
        std::function<void(const std::filesystem::path&)> copy;
        std::function<void(const std::filesystem::path&)> open;
        std::function<void(const std::filesystem::path&)> openInExplorer;
        std::function<void(const std::filesystem::path&)> paste;
        std::function<void(const std::filesystem::path&)> remove;
        std::function<void(const std::filesystem::path&)> rename;
        std::function<bool(const std::filesystem::path&)> isMultiSelection;
        std::function<void(const std::filesystem::path&, bool additive)> select;
    };

    class ThumbnailFile
    {
    public:
        ThumbnailFile(const ThumbnailActions& actions, ImTextureID texture,
                      const std::filesystem::directory_entry& entry, bool isSelected,
                      bool canPaste);

        void draw();

    private:
        void openSelectedPath();
        void requestOpenSelectedPath();

        void drawToolTip();
        void drawImageButton();
        void drawContextMenu();

    private:
        std::filesystem::path _path;
        std::string _fileName;
        ImTextureID _textureId;
        glm::vec2 _size = glm::vec2(70, 70);
        bool _isDirectory = false;
        bool _isRegularFile = false;

        bool _needOpen{ false };
        bool _isSelected{ false };
        bool _canPaste{ false };

        ThumbnailActions _actions;
    };
} // namespace Core
