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

#include "ThumbnailFile.h"

#include "AssetsManager/AssetsManager.h"
#include "Editor/Windows/ModalPopUp.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Misc/IconsFontAwesome.h"

#include <array>
#include <chrono>
#include <format>
#include <iomanip>
#include <sstream>

namespace
{
    std::string PrettyBytes(uint64_t bytes)
    {
        static const std::array<const char*, 6> suffixes = { "B", "KB", "MB", "GB", "TB", "PB" };

        if (bytes == 0)
        {
            return "0 B";
        }

        int i = 0;
        auto count = static_cast<double>(bytes);

        while (count >= 1024.0 && i < static_cast<int>(suffixes.size()) - 1)
        {
            count /= 1024.0;
            ++i;
        }

        std::ostringstream out;
        out << std::fixed << std::setprecision(count < 10 ? 2 : (count < 100 ? 1 : 0)) << count
            << ' ' << suffixes[i];

        return out.str();
    }
} // namespace

namespace Core
{

    ThumbnailFile::ThumbnailFile(const ThumbnailActions& actions, const ImTextureID texture,
                                 const std::filesystem::directory_entry& entry,
                                 const bool isSelected, const bool canPaste)
        : _path{ entry.path() },
          _textureId{ texture },
          _isDirectory{ entry.is_directory() },
          _isRegularFile{ entry.is_regular_file() },
          _isSelected{ isSelected },
          _canPaste{ canPaste },
          _actions{ actions }
    {
        _fileName = _path.filename().generic_string();
    }

    void ThumbnailFile::draw()
    {
        {
            ImGui::BeginGroup();
            drawImageButton();

            requestOpenSelectedPath();

            drawContextMenu();

            const auto labelStartX = ImGui::GetCursorPosX();
            const auto labelSize = ImGui::CalcTextSize(_fileName.c_str(), nullptr, false, _size.x);
            if (labelSize.x < _size.x)
            {
                ImGui::SetCursorPosX(labelStartX + (_size.x - labelSize.x) * 0.5f);
            }

            ImGui::PushTextWrapPos(labelStartX + _size.x);
            ImGui::TextUnformatted(_fileName.c_str());
            ImGui::PopTextWrapPos();

            ImGui::EndGroup();
        }

        if (ImGui::IsItemHovered())
        {
            drawToolTip();
        }

        openSelectedPath();
    }

    void ThumbnailFile::openSelectedPath()
    {
        if (_needOpen)
        {
            if (_isDirectory)
            {
                _actions.open(_path);
            }
            else if (_isRegularFile)
            {
                AssetsManager::TryToOpenFile(std::filesystem::directory_entry(_path));
            }
        }
    }

    void ThumbnailFile::requestOpenSelectedPath()
    {
        if (ImGui::IsItemHovered()
            && (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)
                || ImGui::IsKeyPressed(ImGuiKey_Enter, false)))
        {
            _needOpen = true;
        }
    }

    void ThumbnailFile::drawToolTip()
    {
        // TODO: needs tooltip logic integration

        ImGui::BeginTooltip();

        const auto& path = _path;

        ImGui::Text("Name:      %s", _fileName.c_str());

        {
            std::error_code ec;
            const auto fsTime = std::filesystem::last_write_time(path, ec);

            if (!ec)
            {
                const auto sysTime = std::chrono::clock_cast<std::chrono::system_clock>(fsTime);

                ImGui::Text("Modified:  %s", std::format("{}", sysTime).c_str());
            }
            else
            {
                ImGui::Text("Modified:  N/A");
            }
        }

        ImGui::Text("Location:  %s", path.generic_string().c_str());

        if (_isRegularFile)
        {
            std::error_code ec;
            const auto size = std::filesystem::file_size(path, ec);

            if (!ec)
            {
                ImGui::Text("File size: %s", PrettyBytes(size).c_str());
            }
            else
            {
                ImGui::Text("File size: N/A");
            }
        }

        ImGui::EndTooltip();
    }

    void ThumbnailFile::drawImageButton()
    {
        _size -= ImGui::GetStyle().FramePadding * 2.f;

        const auto styleColor = _isSelected ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered)
                                            : glm::vec4{ 0, 0, 0, 0 };

        ImGui::PushStyleColor(ImGuiCol_Button, styleColor);
        ImGui::ImageButton(_fileName.c_str(), _textureId, _size);
        ImGui::PopStyleColor();

        if (_actions.select)
        {
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                _actions.select(_path, ImGui::GetIO().KeyCtrl);
            }
            else if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
                if (!_isSelected)
                {
                    _actions.select(_path, false);
                }
            }
        }

        _size += ImGui::GetStyle().FramePadding * 2.f;
    }

    void ThumbnailFile::drawContextMenu()
    {
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup(_fileName.c_str());
        }

        if (ImGui::BeginPopup(_fileName.c_str()))
        {
            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN_O " Open"))
            {
                _needOpen = true;
            }
            if (ImGui::MenuItem(ICON_FA_FILES_O " Copy"))
            {
                _actions.copy(_path);
            }
            if (ImGui::MenuItem(ICON_FA_SCISSORS " Cut"))
            {
                _actions.cut(_path);
            }
            if (_canPaste && _isDirectory && ImGui::MenuItem(ICON_FA_ARROW_DOWN " Paste"))
            {
                _actions.paste(_path);
            }

            if (ImGui::MenuItem(ICON_FA_PENCIL " Rename"))
            {
                _actions.rename(_path);
            }
            const bool deleteMultiple
                = _actions.isMultiSelection && _actions.isMultiSelection(_path);

            if (deleteMultiple)
            {
                if (ImGui::MenuItem(ICON_FA_TRASH " Delete selected files"))
                {
                    const auto removeSelected = _actions.removeSelected;

                    ModalPopUp::Open("Do you really want to delete selected files?",
                                     [removeSelected](bool isOk)
                                     {
                                         if (isOk && removeSelected)
                                         {
                                             removeSelected();
                                         }
                                     });
                }
            }
            else
            {
                if (ImGui::MenuItem(ICON_FA_TRASH " Delete"))
                {
                    const auto pathCopy = _path;
                    const auto remove = _actions.remove;
                    const auto isDirectory = _isDirectory;
                    ModalPopUp::Open("Do you really want to delete the {}: {}?"_f
                                         << (isDirectory ? "directory" : "file")
                                         << pathCopy.generic_string(),
                                     [remove, pathCopy](const bool isOk)
                                     {
                                         if (isOk && remove)
                                         {
                                             remove(pathCopy);
                                         }
                                     });
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Open in explorer"))
            {
                if (_actions.openInExplorer)
                {
                    _actions.openInExplorer(_path);
                }
            }

            const auto weakAsset = GetAssetsManager().getWeakEcsAssetByPath(_path.generic_string());
            if (auto asset = weakAsset.tryLoad();
                asset && asset->canProcessAction(AssetAction::AA_Spawn))
            {
                if (ImGui::MenuItem("Spawn on scene"))
                {
                    asset->processAction(AssetAction::AA_Spawn);
                }
            }

            ImGui::EndPopup();
        }
    }

} // namespace Core
