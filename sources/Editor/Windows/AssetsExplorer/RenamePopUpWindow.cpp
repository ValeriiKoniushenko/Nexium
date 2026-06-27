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


#include "RenamePopUpWindow.h"

#include "ImGui/imgui.h"
#include "Misc/IconsFontAwesome.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"

namespace Core
{
    void RenamePopUpWindow::open(const std::filesystem::path& path, const std::string& fileName)
    {
        _needOpen = true;
        _renamePath = path;
        _renameError.clear();
        _renameBuffer = path.filename().generic_string();
    }

    void RenamePopUpWindow::draw()
    {
        drawPopup();
    }

    void RenamePopUpWindow::drawPopup()
    {
        constexpr const char* popupName = ICON_FA_PENCIL
            " Rename";

        if (_needOpen)
        {
            ImGui::OpenPopup(popupName);
            _needOpen = false;
        }

        ImGui::SetNextWindowSize(glm::vec2(360.f, 0.f), ImGuiCond_Appearing);
        if (!ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            return;
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            closeWindow();
            ImGui::EndPopup();
            return;
        }

        ImGui::TextUnformatted("New name");
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::IsWindowAppearing())
        {
            ImGui::SetKeyboardFocusHere();
        }

        const bool enterPressed = ImGui::InputText("##RenameInput", &_renameBuffer,
                                                   ImGuiInputTextFlags_EnterReturnsTrue
                                                   | ImGuiInputTextFlags_AutoSelectAll);
        ImGui::PopItemWidth();

        if (!_renameError.empty())
        {
            ImGui::Dummy({});
            ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "%s",
                               _renameError.c_str());
        }

        ImGui::Dummy({});
        ImGui::Separator();
        ImGui::Dummy({});

        const auto buttonWidth = 120.f;
        if (ImGui::Button("Rename", glm::vec2(buttonWidth, 0.f)) || enterPressed)
        {
            _renameBuffer = TrimWhitespace(_renameBuffer);
            if (renamePath(_renamePath, _renameBuffer))
            {
                _renamePath.clear();
                _renameBuffer.clear();
                _renameError.clear();
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", glm::vec2(buttonWidth, 0.f)))
        {
            _renamePath.clear();
            _renameBuffer.clear();
            _renameError.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    void RenamePopUpWindow::closeWindow()
    {
        _renamePath.clear();
        _renameBuffer.clear();
        _renameError.clear();

        ImGui::CloseCurrentPopup();
    }

    void RenamePopUpWindow::updateSelectedPathAfterRename()
    {
        // for (auto& selectedPath : _selectedPaths)
        // {
        //     if (selectedPath == path)
        //     {
        //         selectedPath = newPath;
        //     }
        // }
        //
        // if (_selectedPath == path)
        // {
        //     _selectedPath = newPath;
        // }
    }

    std::string RenamePopUpWindow::TrimWhitespace(std::string value)
    {
        const auto isNotSpace = [](unsigned char ch) { return !std::isspace(ch); };

        value.erase(value.begin(), std::find_if(value.begin(), value.end(), isNotSpace));
        value.erase(std::find_if(value.rbegin(), value.rend(), isNotSpace).base(), value.end());

        return value;
    }

    bool RenamePopUpWindow::renamePath(const std::filesystem::path& path, const std::string& newName)
    {
        const auto normalizedName = TrimWhitespace(newName);
        if (normalizedName.empty())
        {
            _renameError = "Name can't be empty.";
            return false;
        }

        if (normalizedName.size() > 255)
        {
            _renameError = "Name is too long.";
            return false;
        }

        if (ContainsInvalidFilenameCharacter(normalizedName))
        {
            _renameError = "Name contains invalid characters.";
            return false;
        }

        if (normalizedName.back() == '.')
        {
            _renameError = "Name can't end with a dot.";
            return false;
        }

        const std::filesystem::path newNamePath(normalizedName);
        if (normalizedName == "." || normalizedName == ".." || newNamePath.is_absolute()
            || newNamePath.has_parent_path())
        {
            _renameError = "Name can't contain path separators.";
            return false;
        }

        const auto oldName = path.filename().generic_string();
        if (normalizedName == oldName)
        {
            return true;
        }

        const auto newPath = path.parent_path() / normalizedName;
        if (std::filesystem::exists(newPath))
        {
            _renameError = "A file or folder with this name already exists.";
            return false;
        }

        std::error_code ec;
        std::filesystem::rename(path, newPath, ec);
        if (ec)
        {
            _renameError = ec.message();
            // errorLog("Can't rename file from: '{}' to '{}'. Reason: {}"_f
            //          << oldName << normalizedName << ec.message());
            return false;
        }

        updateSelectedPathAfterRename();

        // refresh();
        return true;
    }

    bool RenamePopUpWindow::ContainsInvalidFilenameCharacter(std::string_view value)
    {
        static constexpr std::string_view invalidChars = "<>:\"/\\|?*";

        return std::ranges::any_of(value,
                                   [](unsigned char ch)
                                   {
                                       return ch < 32 || invalidChars.find(static_cast<char>(ch))
                                           != std::string_view::npos;
                                   });
    }
}
