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

#include "AnimationPreviewPanel.h"

#include <algorithm>

namespace Core
{
    void AnimationPreviewPanel::drawEmptyPreview()
    {
        const auto origin = ImGui::GetCursorScreenPos();
        const auto available = ImGui::GetContentRegionAvail();
        const auto size = glm::vec2{ std::max(available.x, 1.f), std::max(available.y, 1.f) };
        auto* drawList = ImGui::GetWindowDrawList();
        constexpr float cellSize = 16.f;
        const auto dark = ImGui::GetColorU32(ImGuiCol_WindowBg);
        const auto light = ImGui::GetColorU32(ImGuiCol_ChildBg);

        for (int row = 0; static_cast<float>(row) * cellSize < size.y; ++row)
        {
            for (int column = 0; static_cast<float>(column) * cellSize < size.x; ++column)
            {
                const float x = static_cast<float>(column) * cellSize;
                const float y = static_cast<float>(row) * cellSize;
                drawList->AddRectFilled({ origin.x + x, origin.y + y },
                                        { origin.x + std::min(x + cellSize, size.x),
                                          origin.y + std::min(y + cellSize, size.y) },
                                        (row + column) % 2 == 0 ? dark : light);
            }
        }

        const auto message = "Add frames to preview your animation"_atom;
        const auto textSize = ImGui::CalcTextSize(message.c_str());
        drawList->AddText({ origin.x + std::max(0.f, (size.x - textSize.x) * 0.5f),
                            origin.y + std::max(0.f, (size.y - textSize.y) * 0.5f) },
                          ImGui::GetColorU32(ImGuiCol_TextDisabled), message.c_str());
        ImGui::Dummy(size);
    }

    void AnimationPreviewPanel::onDraw()
    {
        if (ImGui::BeginChild("PreviewPanel", { getWidth(), getHeight() }, true))
        {
            ImGui::TextUnformatted("Animation preview");
            ImGui::Separator();
            drawEmptyPreview();
        }
        ImGui::EndChild();
    }

} // namespace Core
