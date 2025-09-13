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

#include "ImageViewer.h"

#include "GameplaySystem/Framework/GameInstance.h"
#include "ImGui/imgui_internal.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace Core
{
    ECS_COMPONENT_IMPL(ImageViewerEWC)

    void ImageViewerEWC::openImageFromFile(const std::filesystem::path& path)
    {
        if (_image.loadFromFile(path, false))
        {
            _path = path.generic_string();
        }
    }

    void ImageViewerEWC::putArguments(const StringAtom& args)
    {
        BaseFloatEWC::putArguments(args);

        openImageFromFile(args.c_str());
    }

    void ImageViewerEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();
    }

    void ImageViewerEWC::onUpdate()
    {
    }

    void ImageViewerEWC::onDraw()
    {
        if (!_image.isValid())
        {
            return;
        }

        if (ImGui::IsWindowHovered() && ImGui::GetIO().MouseWheel != 0.0f)
        {
            _zoom = std::max(0.1f, _zoom + ImGui::GetIO().MouseWheel * 0.1f);
        }

        // Middle mouse drag to pan
        if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
        {
            const auto drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);
            _offset.x += drag.x;
            _offset.y += drag.y;
        }
        const auto displaySize
            = glm::vec2(_image.getSize().width * _zoom, _image.getSize().height * _zoom);
        ImGui::SetCursorPos(ImGui::GetCursorPos() + _offset);

        ImGui::Text("%s: %dx%d", _path.c_str(), _image.getSize().width, _image.getSize().height);
        ImGui::Image(_image.getTextureId(), displaySize);
    }

} // namespace Core
