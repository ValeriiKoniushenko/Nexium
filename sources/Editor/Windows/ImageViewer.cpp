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

#include "ImageViewer.h"

#include "Misc/IconsFontAwesome.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "ImGui/imgui.h"
#include "glm/gtx/string_cast.hpp"

#include <algorithm>
#include <cmath>

namespace Core
{
    ECS_COMPONENT_IMPL(ImageViewerEWC);
    R_FRIEND_IMPL(ImageViewerEWC);

    const char* ImageViewerEWC::getIcon()
    {
        return ICON_FA_PICTURE_O;
    }

    void ImageViewerEWC::openImageFromFile(const std::filesystem::path& path)
    {
        if (!_image)
        {
            _image = std::make_unique<Texture>();
        }

        if (_image->loadFromFile(path, false))
        {
            _path = path.generic_string();
            _windowTitle = "Image Viewer — {}###ImageViewer"_f << _path;
            _offset = {};
            _zoom = 1.0f;
            _fitOnNextDraw = true;
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

        _windowFlags |= ImGuiWindowFlags_NoScrollbar;
        _windowFlags |= ImGuiWindowFlags_NoScrollWithMouse;
    }

    void ImageViewerEWC::onUpdate()
    {
        BaseFloatEWC::onUpdate();
    }

    float ImageViewerEWC::calculateFitZoom(const glm::vec2& canvasSize,
                                           const glm::vec2& rawImageSize)
    {
        return std::min({ 1.0f, canvasSize.x / rawImageSize.x, canvasSize.y / rawImageSize.y });
    }

    void ImageViewerEWC::handleZoom(bool isCanvasHovered, const glm::vec2& canvasPosition,
                                    const glm::vec2& canvasSize, const glm::vec2& rawImageSize,
                                    float fitZoom)
    {
        const auto& io = ImGui::GetIO();
        if (!isCanvasHovered || !io.KeyCtrl || io.MouseWheel == 0.0f)
        {
            return;
        }

        const glm::vec2 oldDisplaySize = rawImageSize * _zoom;
        const glm::vec2 oldImagePosition
            = canvasPosition + (canvasSize - oldDisplaySize) * 0.5f + _offset;
        const glm::vec2 mousePosition = ImGui::GetMousePos();

        const bool isMouseOverImage = mousePosition.x >= oldImagePosition.x
                                      && mousePosition.y >= oldImagePosition.y
                                      && mousePosition.x <= oldImagePosition.x + oldDisplaySize.x
                                      && mousePosition.y <= oldImagePosition.y + oldDisplaySize.y;
        const glm::vec2 anchor = isMouseOverImage
                                     ? (mousePosition - oldImagePosition) / oldDisplaySize
                                     : glm::vec2(0.5f);
        const glm::vec2 anchorPosition
            = isMouseOverImage ? mousePosition : oldImagePosition + oldDisplaySize * 0.5f;

        constexpr float zoomStep = 1.15f;
        _zoom = std::max(_zoom * std::pow(zoomStep, io.MouseWheel), fitZoom);

        const glm::vec2 newDisplaySize = rawImageSize * _zoom;
        const glm::vec2 centeredImagePosition
            = canvasPosition + (canvasSize - newDisplaySize) * 0.5f;
        _offset = anchorPosition - anchor * newDisplaySize - centeredImagePosition;
    }

    void ImageViewerEWC::handlePan(bool isCanvasHovered)
    {
        if (!isCanvasHovered || !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            return;
        }

        const auto drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
        _offset += drag;
    }

    void ImageViewerEWC::clampOffset(const glm::vec2& rawImageSize, float fitZoom)
    {
        const glm::vec2 displaySize = rawImageSize * _zoom;
        const glm::vec2 fittedImageSize = rawImageSize * fitZoom;
        const glm::vec2 maxOffset = (displaySize - fittedImageSize) * 0.5f;
        _offset = glm::clamp(_offset, -maxOffset, maxOffset);
    }

    void ImageViewerEWC::drawImage(const glm::vec2& canvasPosition, const glm::vec2& canvasSize,
                                   const glm::vec2& rawImageSize) const
    {
        const glm::vec2 displaySize = rawImageSize * _zoom;
        const glm::vec2 imagePosition
            = canvasPosition + (canvasSize - displaySize) * 0.5f + _offset;

        auto* drawList = ImGui::GetWindowDrawList();
        drawList->PushClipRect(canvasPosition, canvasPosition + canvasSize, true);
        drawList->AddImage(_image->getTextureId(), imagePosition, imagePosition + displaySize);
        drawList->PopClipRect();
    }

    void ImageViewerEWC::onDraw()
    {
        if (!_image || (_image && !_image->isValid()))
        {
            return;
        }

        const glm::vec2 canvasPosition = ImGui::GetCursorScreenPos();
        glm::vec2 canvasSize = ImGui::GetContentRegionAvail();
        if (canvasSize.x <= 0.0f || canvasSize.y <= 0.0f)
        {
            return;
        }

        const glm::vec2 rawImageSize = { static_cast<float>(_image->getSize().width),
                                         static_cast<float>(_image->getSize().height) };
        if (rawImageSize.x <= 0.0f || rawImageSize.y <= 0.0f)
        {
            return;
        }

        const float fitZoom = calculateFitZoom(canvasSize, rawImageSize);
        if (_fitOnNextDraw)
        {
            _zoom = fitZoom;
            _offset = {};
            _fitOnNextDraw = false;
        }
        else
        {
            _zoom = std::max(_zoom, fitZoom);
        }

        ImGui::InvisibleButton("##ImageViewerCanvas", canvasSize, ImGuiButtonFlags_MouseButtonLeft);
        const bool isCanvasHovered = ImGui::IsItemHovered();

        handleZoom(isCanvasHovered, canvasPosition, canvasSize, rawImageSize, fitZoom);
        handlePan(isCanvasHovered);
        clampOffset(rawImageSize, fitZoom);
        drawImage(canvasPosition, canvasSize, rawImageSize);
    }

    void DummyEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();
    }

    void DummyEWC::onUpdate()
    {
    }

    void DummyEWC::onDraw()
    {
    }
} // namespace Core
