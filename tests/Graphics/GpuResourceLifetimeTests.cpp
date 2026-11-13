// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Graphics/GLContextTest.h"
#include "RawBackend/Graphics/Texture.h"

#include <array>

namespace
{
    class GpuResourceLifetimeTests : public GLContextTest
    {
    };

    constexpr std::array<unsigned char, 4> kPixel = { 255, 0, 255, 255 };
} // namespace

TEST_F(GpuResourceLifetimeTests, ReleasingATextureFilledWithPutImageDeletesIt)
{
    // Glyph textures (Font.cpp) and the texture atlas are filled through putImage() rather
    // than loadFromFile/loadFromImage; whatever bookkeeping those paths differ in, a release
    // (or the destructor) must hand the GL name back.
    RawBackend::Texture texture;
    texture.generate();
    texture.bind();
    texture.putImage(0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, kPixel.data());
    const GLuint id = texture.getTextureId();
    ASSERT_NE(id, 0u);
    ASSERT_TRUE(glIsTexture(id));

    texture.release();

    EXPECT_FALSE(texture.isValid());
    EXPECT_FALSE(glIsTexture(id));
}

TEST_F(GpuResourceLifetimeTests, DestroyingAGeneratedButEmptyTextureDeletesIt)
{
    GLuint id = 0;
    {
        RawBackend::Texture texture;
        texture.generate();
        texture.bind(); // glGen* only reserves the name; binding makes it a texture object.
        id = texture.getTextureId();
        ASSERT_TRUE(glIsTexture(id));
    }

    EXPECT_FALSE(glIsTexture(id));
}
