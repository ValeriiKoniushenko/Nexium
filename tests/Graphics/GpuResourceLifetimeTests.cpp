// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Graphics/GLContextTest.h"
#include "NxSubsystems/Graphics/GraphicsComponents.h"
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

TEST_F(GpuResourceLifetimeTests, GeneratingGraphicsDataTwiceKeepsTheFirstBuffers)
{
    // generate() is guarded by !isValid(), but isValid() also requires a shader, so a second
    // generate() before setShader() silently requests fresh VAO/VBO/EBO (and texture) names
    // and loses the first set. Generation must be idempotent.
    NX::BaseGraphicsData data;
    data.generate();
    const GLuint vao = data.getVaoId();
    const GLuint vbo = data.getVboId();
    const GLuint ebo = data.getEboId();
    ASSERT_NE(vao, 0u);

    data.generate();

    EXPECT_EQ(data.getVaoId(), vao);
    EXPECT_EQ(data.getVboId(), vbo);
    EXPECT_EQ(data.getEboId(), ebo);

    NX::SeparTextureGraphicsData textured;
    textured.generate();
    const GLuint texturedVao = textured.getVaoId();
    const GLuint texture = textured.getTextureId();
    const GLuint textureVbo = textured.getTextureVboId();
    ASSERT_NE(texture, 0u);

    textured.generate();

    EXPECT_EQ(textured.getVaoId(), texturedVao);
    EXPECT_EQ(textured.getTextureId(), texture);
    EXPECT_EQ(textured.getTextureVboId(), textureVbo);
}

TEST_F(GpuResourceLifetimeTests, ClearingGraphicsDataDeletesItsBuffers)
{
    NX::SeparTextureGraphicsData data;
    data.generate();
    data.bindAllBuffers(); // glGen* reserves names; binding turns them into objects.
    const GLuint vao = data.getVaoId();
    const GLuint vbo = data.getVboId();
    const GLuint ebo = data.getEboId();
    const GLuint textureVbo = data.getTextureVboId();
    data.bindTexture();
    const GLuint texture = data.getTextureId();
    ASSERT_TRUE(glIsVertexArray(vao));
    ASSERT_TRUE(glIsBuffer(vbo));

    data.clear();

    EXPECT_FALSE(glIsVertexArray(vao));
    EXPECT_FALSE(glIsBuffer(vbo));
    EXPECT_FALSE(glIsBuffer(ebo));
    EXPECT_FALSE(glIsBuffer(textureVbo));
    EXPECT_FALSE(glIsTexture(texture));
    EXPECT_EQ(data.getVaoId(), 0u);
}
