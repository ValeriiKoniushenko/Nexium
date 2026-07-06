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

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "ImGui/imgui.h"

#include <algorithm>
#include <ranges>
#include <utility>

namespace
{
    std::string TrimWhitespace(std::string value)
    {
        const auto isNotSpace = [](unsigned char ch) { return !std::isspace(ch); };

        value.erase(value.begin(), std::ranges::find_if(value, isNotSpace));
        value.erase(std::ranges::find_if(std::views::reverse(value), isNotSpace).base(),
                    value.end());

        return value;
    }
} // namespace

namespace Core
{
    ECS_COMPONENT_IMPL(RenamePopUpWindow);

    void RenamePopUpWindow::open(const StringAtom& text, const std::filesystem::path& path,
                                 std::function<void(const std::filesystem::path& oldPath,
                                                    const std::filesystem::path& newPath)>
                                     onRenameCallback)
    {
        initialize();
        enable();

        if (_hasOpenRequest)
        {
            warnLog(
                "Can't open second time RenamePopUpWindow. It's already processing the "
                "request.");
            return;
        }
        _caption = text;
        _hasOpenRequest = true;
        _onRenameCallback = std::move(onRenameCallback);
        _renameToPath = path;
        _renameError.clear();
        _renameBuffer = path.filename().generic_string();

        if (_fileNameInput)
        {
            _fileNameInput->setInputtedData(_renameBuffer);
            _fileNameInput->requestFocus();
            _fileNameInput->requestSelectAll();
        }
    }

    void RenamePopUpWindow::Open(const StringAtom& text, const std::filesystem::path& path,
                                 std::function<void(const std::filesystem::path& oldPath,
                                                    const std::filesystem::path& newPath)>
                                     onRenameCallback)
    {
        GetEditor().tryToOpenWindow<RenamePopUpWindow>(".*", std::move(text), path,
                                                       onRenameCallback);
    }

    void RenamePopUpWindow::onDraw()
    {
        _layout.tick(GetWorld().timeDelta);

        if (!_renameError.empty())
        {
            ImGui::Dummy({});
            ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "%s",
                               _renameError.c_str());
        }

        ImGui::Dummy({});

        if (ImGui::IsKeyPressed(ImGuiKey_Enter, false)
            || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false))
        {
            _renameBuffer = TrimWhitespace(_fileNameInput->getInputtedData());
            if (renamePath(_renameBuffer))
            {
                _renameToPath.clear();
                _renameBuffer.clear();
                _renameError.clear();
                closeWindow();
                ImGui::CloseCurrentPopup();
            }
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
        {
            cancelChangesAndCloseWindow();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Enter, false))
        {
            applyChangesAndCloseWindow();
        }
    }

    void RenamePopUpWindow::onInitialize()
    {
        BaseEWC::onInitialize();

        _label = _layout.addChildComponent<Gui::Label>("New name");
        _fileNameInput = _layout.addChildComponent<Gui::TextInput>();
        _fileNameInput->setFlex(Gui::Flex::FlexWidth);
        _fileNameInput->setInputtedData(_renameBuffer);
        if (ImGui::IsWindowAppearing())
        {
            ImGui::SetKeyboardFocusHere();
        }

        auto* hLayout = _layout.addChildComponent<Gui::HorizontalLayout>();

        _applyButton = hLayout->addChildComponent<Gui::Button>("Apply");
        _applyButton->setFlex(Gui::Flex::FlexWidth);
        _cancelButton = hLayout->addChildComponent<Gui::Button>("Cancel");
        _cancelButton->setFlex(Gui::Flex::FlexWidth);

        _subscriptionPool << _applyButton->onClick->subscribeAndGetID(
            [this]() { applyChangesAndCloseWindow(); });
        _subscriptionPool << _cancelButton->onClick->subscribeAndGetID(
            [this]() { cancelChangesAndCloseWindow(); });
    }

    void RenamePopUpWindow::preOpenedEndWindowDraw()
    {
        ImGui::EndPopup();
    }

    bool RenamePopUpWindow::beginWindowDraw()
    {
        if (_hasOpenRequest)
        {
            ImGui::OpenPopup(_caption.c_str());
            ImGui::SetNextWindowSize(glm::vec2(400, 0), ImGuiCond_Appearing);
            _hasOpenRequest = false;
        }
        return ImGui::BeginPopupModal(_caption.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
    }

    void RenamePopUpWindow::endWindowDraw()
    {
    }

    void RenamePopUpWindow::onClose()
    {
        BaseEWC::onClose();
        _renameToPath.clear();
        _renameBuffer.clear();
        _renameError.clear();
        ImGui::CloseCurrentPopup();
    }

    void RenamePopUpWindow::applyChangesAndCloseWindow()
    {
        _renameBuffer = TrimWhitespace(_fileNameInput->getInputtedData());
        if (renamePath(_renameBuffer))
        {
            closeWindow();
        }
    }

    void RenamePopUpWindow::cancelChangesAndCloseWindow()
    {
        closeWindow();
    }

    bool RenamePopUpWindow::renamePath(const std::string& newName)
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

        const auto oldName = _renameToPath.filename().generic_string();
        if (normalizedName == oldName)
        {
            return true;
        }

        const auto newPath = _renameToPath.parent_path() / normalizedName;
        if (std::filesystem::exists(newPath))
        {
            _renameError = "A file or folder with this name already exists.";
            return false;
        }

        std::error_code ec;
        std::filesystem::rename(_renameToPath, newPath, ec);
        if (ec)
        {
            _renameError = ec.message();
            errorLog("Can't rename file from: '{}' to '{}'. Reason: {}"_f
                     << oldName << normalizedName << ec.message());
            return false;
        }

        if (_onRenameCallback)
        {
            _onRenameCallback(_renameToPath, newPath);
        }
        return true;
    }

    bool RenamePopUpWindow::ContainsInvalidFilenameCharacter(std::string_view value)
    {
        static constexpr std::string_view invalidChars = "<>:\"/\\|?*";

        return std::ranges::any_of(value,
                                   [](unsigned char ch)
                                   {
                                       return ch < 32
                                              || invalidChars.find(static_cast<char>(ch))
                                                     != std::string_view::npos;
                                   });
    }
} // namespace Core
