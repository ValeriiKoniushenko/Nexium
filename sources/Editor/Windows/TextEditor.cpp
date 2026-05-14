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

#include "TextEditor.h"

#include "ImGui/imgui.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "Misc/IconsFontAwesome.h"
#include "Utils/Functions.h"

namespace Core
{
    ECS_COMPONENT_IMPL(TextEditorEWC);
    R_FRIEND_IMPL(TextEditorEWC);

    const char* TextEditorEWC::getIcon()
    {
        return ICON_FA_FILE_TEXT_O;
    }

    void TextEditorEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        _minWindowSize = FSize2(500.f, 500.f);
        _windowFlags |= ImGuiWindowFlags_MenuBar;
    }

    void TextEditorEWC::onDraw()
    {
        drawBarMenu();

        drawEditor();

        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
        {
            bool const ctrl = ImGui::GetIO().KeyCtrl;
            if (ctrl && ImGui::IsKeyPressed(ImGuiKey_S, false))
            {
                save();
            }
        }
    }

    void TextEditorEWC::putArguments(const StringAtom& args)
    {
        BaseEWC::putArguments(args);

        _path = args.data();
        if (_path.empty())
        {
            return;
        }

        _fileContent = Utils::TryToGetTextFileContentAs<std::string>(_path);
        if (_fileContent.empty())
        {
            errorLog("TextEditor can't open file: {}"_f << _path.generic_string());
            return;
        }

        setComponentName(_path.filename().generic_string().data());
        _wasEdited = false;
    }

    void TextEditorEWC::drawBarMenu()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem(ICON_FA_FLOPPY_O " Save"))
                {
                    save();
                }
                if (ImGui::MenuItem(ICON_FA_POWER_OFF " Exit"))
                {
                    closeWindow();
                }

                ImGui::EndMenu();
            }

            if (_wasEdited)
            {
                ImGui::SameLine(ImGui::GetWindowWidth()
                                - (ImGui::CalcTextSize(ICON_FA_TIMES_CIRCLE).x + _statusMarginRight)
                                - ImGui::GetStyle().ItemSpacing.x * 3.f);
                ImGui::TextUnformatted(ICON_FA_TIMES_CIRCLE);
            }
            else
            {
                ImGui::SameLine(ImGui::GetWindowWidth()
                                - (ImGui::CalcTextSize(ICON_FA_CHECK_CIRCLE).x + _statusMarginRight)
                                - ImGui::GetStyle().ItemSpacing.x * 3.f);
                ImGui::TextUnformatted(ICON_FA_CHECK_CIRCLE);
            }

            ImGui::EndMenuBar();
        }
    }

    void TextEditorEWC::save()
    {
        if (_path.empty())
        {
            return;
        }

        errno = 0;

        std::ofstream file(_path);
        if (!file.is_open())
        {
            int const err = errno;
            char err_buf[256];
#if defined(_MSC_VER)
            strerror_s(err_buf, sizeof(err_buf), err);
#else
            std::strncpy(err_buf, std::strerror(err), sizeof(err_buf));
            err_buf[sizeof(err_buf) - 1] = '\0';
#endif

            errorLog("File: {} - wasn't opened. Reason: {}"_f << _path << err_buf);
            return;
        }

        file.write(_fileContent.c_str(), _fileContent.size() * sizeof(_fileContent[0]));
        _wasEdited = false;
    }

    void TextEditorEWC::drawEditor()
    {
        ImGui::PushTextWrapPos();
        if (ImGui::InputTextMultiline("##editor", &_fileContent,
                                      glm::vec2(_innerSize.width, _innerSize.height)))
        {
            _wasEdited = true;
        }
        ImGui::PopTextWrapPos();
    }
} // namespace Core