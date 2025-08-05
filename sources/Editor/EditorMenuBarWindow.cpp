// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "EditorMenuBarWindow.h"

#include "GameplaySystem/Framework/GameInstance.h"

namespace SW
{

    void EditorMenuBarWindow::onDraw()
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open"))
            {
            }
            if (ImGui::MenuItem("Save"))
            {
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows"))
        {
            if (ImGui::MenuItem("Keyboard shortcuts"))
            {
                gameInstance->gameEditor.showWindow<KeyboardShortcutsEWC>(".*");
            }
            if (ImGui::MenuItem("Viewport"))
            {
                gameInstance->gameEditor.showWindow<GameViewportEWC>("Viewport");
            }
            if (ImGui::MenuItem("Logs"))
            {
                gameInstance->gameEditor.showWindow<LogsWindow>("Logs");
            }
            ImGui::EndMenu();
        }

        ImGui::SameLine(ImGui::GetWindowWidth()
                        - (ImGui::CalcTextSize(_fpsTextTemplate).x + _fpsMarginRight)
                        - ImGui::GetStyle().ItemSpacing.x);
        ImGui::TextUnformatted(_cachedFpsText.c_str());
    }

    void EditorMenuBarWindow::onUpdate()
    {
        BaseMenuBarEWC::onUpdate();

        _cachedFpsText
            = _fpsText + Core::StringAtom::MakeFrom(static_cast<int>(ImGui::GetIO().Framerate));
    }

} // namespace SW