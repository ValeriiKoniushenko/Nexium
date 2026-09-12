// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

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
