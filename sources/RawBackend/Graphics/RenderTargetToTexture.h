// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Size.h"
#include "Platform/Graphics.h"

namespace NX
{
    class RenderTargetToTexture
    {
    public:
        void generate();

        void clearCanvas() const;

        void callMePreDraw() const;

        void callMeAfterDraw() const;

        [[nodiscard]] GLuint getTextureId() const noexcept { return _tex; }
        [[nodiscard]] Core::ISize2 getRenderSize() const noexcept { return _size; }

        void setRenderSize(Core::ISize2 size);

        void destroy();

    private:
        Core::ISize2 _size = Core::ISize2{ 400, 400 };
        GLuint _fbo = 0, _tex = 0, _rbo = 0;
        bool _isGenerated = false;
    };
} // namespace NX
