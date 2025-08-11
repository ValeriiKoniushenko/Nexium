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

#include "RootDockWindow.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(RootDockWindowEWC)

    // ========================================================================
    //
    //    ______                _   ______               _
    //    | ___ \              | |  |  _  \             | |
    //    | |_/ /  ___    ___  | |_ | | | |  ___    ___ | | __
    //    |    /  / _ \  / _ \ | __|| | | | / _ \  / __|| |/ /
    //    | |\ \ | (_) || (_) || |_ | |/ / | (_) || (__ |   <
    //    \_| \_| \___/  \___/  \__||___/   \___/  \___||_|\_\
    // ========================================================================
    void RootDockWindowEWC::onInit()
    {
        BaseEWC::onInit();

        _windowFlags |= ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
                        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    void RootDockWindowEWC::onDraw()
    {
        ImGuiID dockSpaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    }

    bool RootDockWindowEWC::beginWindowDraw()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        const auto res = ImGui::Begin(getComponentName().c_str(), &_isEnabled, _windowFlags);

        ImGui::PopStyleVar(2);

        return res;
    }

    void RootDockWindowEWC::endWindowDraw()
    {
        ImGui::End();
    }

} // namespace Core
