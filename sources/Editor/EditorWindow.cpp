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

#include "EditorWindow.h"

#include "GameplaySystem/Framework/GameInstance.h"
#include "ImGui/backends/imgui_impl_glfw.h"

namespace SW
{

    void BaseEWC::onTick()
    {
        if (beginWindowDraw())
        {
            onUpdate(); // in the future maybe will be called not every tick
            onDraw();
        }
        endWindowDraw();
    }

    void BaseFloatEWC::setFitContent(bool v)
    {
        _isFitContent = v;
        if (v)
        {
            _windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
        }
        else
        {
            _windowFlags &= ~ImGuiWindowFlags_AlwaysAutoResize;
        }
    }

    void BaseFloatEWC::onUpdate()
    {
        const ImVec2 size = ImGui::GetWindowSize();
        _size = Core::FSize2{ size.x, size.y };
    }

    void GameViewportEWC::onInit()
    {
        BaseEWC::onInit();

        setComponentName("Viewport");
    }

    bool BaseFloatEWC::beginWindowDraw()
    {
        if (_isFitContent)
        {
            ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
        }

        return ImGui::Begin(getComponentName().c_str(), &_isEnabled, _windowFlags);
    }

    void BaseFloatEWC::endWindowDraw()
    {
        ImGui::End();
    }

    void GameViewportEWC::onDraw()
    {
        if (gameInstance->renderMode.cast() == GameInstance::RenderMode::ToTexture)
        {
            const auto& r = gameInstance->renderToTextureObject;
            const ImVec2 renderSize = { static_cast<float>(r.getRenderSize().width),
                                        static_cast<float>(r.getRenderSize().height) };
            ImGui::Image(r.getTextureId(), renderSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
        }
    }

    void KeyboardShortcutsEWC::onInit()
    {
        BaseFloatEWC::onInit();

        setComponentName("Keyboard Shortcuts");
    }

    void KeyboardShortcutsEWC::onDraw()
    {
        ImGui::Text("Shortcuts:");
        ImGui::Text("    F12     - Toggle render mode");
        ImGui::Text("    W/A/S/D - Move Control");
        ImGui::Text("    C/Space - Down/Up");
        ImGui::Text("    Esc     - exit");
        ImGui::Text("");
        ImGui::Text("Globals:");
        ImGui::Text("Mouse sensitivity");
        ImGui::SliderFloat("##mouseSensitivity", &gameInstance->mouseSensitivity, 100.f, 1300.f);
        ImGui::Text("Spectator speed");
        ImGui::SliderFloat("##spectator_speed", &gameInstance->speed, 1.f, 300.f);
    }
    void RootDockWindow::onInit()
    {
        BaseEWC::onInit();

        setComponentName("Root dock space");

        _windowFlags |= ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
                        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    void RootDockWindow::onDraw()
    {
        ImGuiID dockSpaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    }

    bool RootDockWindow::beginWindowDraw()
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        const auto res = ImGui::Begin(getComponentName().c_str(), &_isEnabled, _windowFlags);

        ImGui::PopStyleVar(2);

        return res;
    }

    void RootDockWindow::endWindowDraw()
    {
        ImGui::End();
    }

    void BaseMenuBarEWC::onInit()
    {
        BaseEWC::onInit();

        setComponentName("Menu Bar");
    }

    bool BaseMenuBarEWC::beginWindowDraw()
    {
        return ImGui::BeginMainMenuBar();
    }

    void BaseMenuBarEWC::endWindowDraw()
    {
        ImGui::EndMainMenuBar();
    }
} // namespace SW