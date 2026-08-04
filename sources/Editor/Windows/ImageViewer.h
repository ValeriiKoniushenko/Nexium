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

#pragma once

#include "BaseWindow.h"
#include "Graphics/Texture.h"

namespace Core
{
    CLASS();
    class ImageViewerEWC : public BaseFloatEWC
    {
        ECS_DECL(ImageViewerEWC, Core::BaseFloatEWC);

    public:
        void openImageFromFile(const std::filesystem::path& path);

        void putArguments(const StringAtom& args) override;

        [[nodiscard]] const char* getIcon() override;

    protected:
        void onInitialize() override;

        void onUpdate() override;

        void onDraw() override;

    private:
        [[nodiscard]] static float calculateFitZoom(const glm::vec2& canvasSize,
                                                    const glm::vec2& rawImageSize);

        void handleZoom(bool isCanvasHovered, const glm::vec2& canvasPosition,
                        const glm::vec2& canvasSize, const glm::vec2& rawImageSize, float fitZoom);

        void handlePan(bool isCanvasHovered);

        void clampOffset(const glm::vec2& rawImageSize, float fitZoom);

        void drawImage(const glm::vec2& canvasPosition, const glm::vec2& canvasSize,
                       const glm::vec2& rawImageSize) const;

    protected:
        std::unique_ptr<Texture> _image;
        std::string _path;
        glm::vec2 _offset = {};
        float _zoom = 1.0f;
        bool _fitOnNextDraw = true;
    };

    CLASS();
    class DummyEWC : public BaseFloatEWC
    {
        ECS_DECL(DummyEWC, Core::BaseFloatEWC);

    public:
        [[nodiscard]] const char* getIcon() override;

    protected:
        void onInitialize() override;
        void onUpdate() override;
        void onDraw() override;

    protected:
    };

} // namespace Core

#include "ImageViewer.generated.h" // added by the code generator. Better don't move it.
