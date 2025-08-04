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

        if (_size != _oldSize)
        {
            const auto regionMax = ImGui::GetContentRegionMax();
            const auto regionMin = ImGui::GetWindowContentRegionMin();
            _innerSize = Core::FSize2(regionMax.x - regionMin.x, regionMax.y - regionMin.y);
            onSizeChanged.trigger(_size, _innerSize);
            _oldSize = _size;
        }
    }

    void GameViewportEWC::onInit()
    {
        BaseEWC::onInit();
    }

    bool BaseFloatEWC::beginWindowDraw()
    {
        if (_isFitContent)
        {
            ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
        }

        for (auto [style, val] : _styles)
        {
            ImGui::PushStyleVar(style, val);
        }

        const auto res = ImGui::Begin(getComponentName().c_str(), &_isEnabled, _windowFlags);

        ImGui::PopStyleVar(_styles.size());

        return res;
    }

    void BaseFloatEWC::endWindowDraw()
    {
        ImGui::End();
    }

    void GameViewportEWC::onDraw()
    {
        if (gameInstance->renderMode.cast() == GameInstance::RenderMode::Editor)
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
    }

    void KeyboardShortcutsEWC::onDraw()
    {
        ImGui::Text("Shortcuts:");
        ImGui::Text("    F12     - Toggle render mode");
        ImGui::Text("    W/A/S/D - Move Control");
        ImGui::Text("    C/Space - Down/Up");
        ImGui::Text("    M       - Mouse capture");
        ImGui::Text("    Alt     - Hold to suppress a mouse");
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

    void RootDockWindow::endWindowDraw()
    {
        ImGui::End();
    }

    void LogsWindow::addLog(Core::StringAtom log)
    {
        _logs.push_back(std::move(log.shrink_to_fit()));
    }

    void LogsWindow::clearLogs()
    {
        _logs.clear();
    }

    void LogsWindow::onInit()
    {
        BaseFloatEWC::onInit();
    }

    void LogsWindow::onDraw()
    {
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), ImGuiChildFlags_NavFlattened,
                              ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::Selectable("Clear"))
                {
                    clearLogs();
                }
                ImGui::EndPopup();
            }

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
            for (const auto& log : _logs)
            {
                ImVec4 color;
                bool has_color = false;
                if (strstr(log.c_str(), "[C]"))
                {
                    // red
                    color = ImVec4(229.f / 255.f, 81.f / 141.f, 0, 1.0f);
                    has_color = true;
                }
                if (strstr(log.c_str(), "[E]"))
                {
                    // orange
                    color = ImVec4(230.f / 255.f, 230.f / 141.f, 27.f / 255.f, 1.0f);
                    has_color = true;
                }
                if (strstr(log.c_str(), "[D]"))
                {
                    // gray
                    color = ImVec4(190.f / 255.f, 190.f / 141.f, 190.f / 255.f, 1.0f);
                    has_color = true;
                }

                if (has_color)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                }

                ImGui::TextUnformatted(log.c_str());

                if (has_color)
                {
                    ImGui::PopStyleColor();
                }
            }

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
    }

    void LogsWindow::onUpdate()
    {
        BaseFloatEWC::onUpdate();

        auto& q = LogQueue::instance();
        while (!q.isEmpty())
        {
            _logs.push_back(std::move(q.frontAndPop().toString()));
        }
    }

    void BaseMenuBarEWC::onInit()
    {
        BaseEWC::onInit();
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