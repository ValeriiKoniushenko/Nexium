// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BaseWindow.h"
#include "RawBackend/Graphics/Texture.h"

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
