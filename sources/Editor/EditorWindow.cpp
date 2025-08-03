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

#include "../../game-example/TemplateGameInstance.h"
#include "ImGui/backends/imgui_impl_glfw.h"

namespace SW
{

    void BaseEditorWindowComponent::onTick()
    {
        if (beginWindowDraw())
        {
            onUpdate(); // in the future maybe will be called not every tick
            onDraw();
        }
        endWindowDraw();
    }

    void BaseFloatEditorWindowComponent::onUpdate()
    {
        const ImVec2 size = ImGui::GetWindowSize();
        _size = Core::FSize2{ size.x, size.y };
    }

    void GameViewportWindow::onInit()
    {
        BaseEditorWindowComponent::onInit();

        _windowTitle = "Viewport";
    }

    bool GameViewportWindow::beginWindowDraw()
    {
        return ImGui::Begin(_windowTitle.c_str(), &_pOpen, _windowFlags);
    }

    void GameViewportWindow::endWindowDraw()
    {
        ImGui::End();
    }

    void GameViewportWindow::onDraw()
    {
        if (gameInstance->renderMode.cast() == GameInstance::RenderMode::ToTexture)
        {
            const auto& r = gameInstance->renderToTextureObject;
            const ImVec2 renderSize = { static_cast<float>(r.getRenderSize().width),
                                        static_cast<float>(r.getRenderSize().height) };
            ImGui::Image(r.getTextureId(), renderSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
        }
    }

    void BaseMenuBarWindowComponent::onInit()
    {
        BaseEditorWindowComponent::onInit();

        _windowTitle = "Menu Bar";
    }

    bool BaseMenuBarWindowComponent::beginWindowDraw()
    {
        return ImGui::BeginMainMenuBar();
    }

    void BaseMenuBarWindowComponent::endWindowDraw()
    {
        ImGui::EndMainMenuBar();
    }
} // namespace SW