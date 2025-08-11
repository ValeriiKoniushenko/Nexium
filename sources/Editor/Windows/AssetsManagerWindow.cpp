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

#include "AssetsManagerWindow.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(AssetsManagerWindowEWC)

    void AssetsManagerWindowEWC::onInit()
    {
        BaseFloatEWC::onInit();
    }

    void AssetsManagerWindowEWC::onDraw()
    {
        drawExplorerTree();
        ImGui::SameLine();
        drawExplorer();
    }

    void AssetsManagerWindowEWC::onUpdate()
    {
        BaseFloatEWC::onUpdate();
    }

    void AssetsManagerWindowEWC::drawExplorerTree()
    {
        if (ImGui::BeginChild("Eplorer tree", ImVec2(200.0f, 0), ImGuiChildFlags_ResizeX))
        {
            ImGui::Text("Hello world! LEFT");
        }
        ImGui::EndChild();
    }

    void AssetsManagerWindowEWC::drawExplorer()
    {
        if (ImGui::BeginChild("Explorer"))
        {
            ImGui::Text("Hello world! Right");
        }
        ImGui::EndChild();
    }
} // namespace Core
