#include "Graphics/Texture.h"

#include "gtest/gtest.h"

namespace
{
    class TextureUploadTest : public ::testing::Test
    {
    protected:
        class TestTexture final : public Core::Texture
        {
        public:
            TestTexture() { _textureId = 1; }
            ~TestTexture() override { _textureId = 0; }
        };

        void SetUp() override
        {
            _originalUpload = glad_glTexImage2D;
            glad_glTexImage2D = &ignoreUpload;
        }

        void TearDown() override { glad_glTexImage2D = _originalUpload; }

        static void APIENTRY ignoreUpload(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum,
                                          GLenum, const void*)
        {
        }

        PFNGLTEXIMAGE2DPROC _originalUpload = nullptr;
        TestTexture texture;
    };

    TEST_F(TextureUploadTest, AtlasAllocationRecordsTextureDimensions)
    {
        texture.putImage(0, GL_RGBA, 256, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        EXPECT_EQ(texture.getSize().width, 256);
        EXPECT_EQ(texture.getSize().height, 128);

        texture.putImage(0, GL_RGBA, 512, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        EXPECT_EQ(texture.getSize().width, 512);
        EXPECT_EQ(texture.getSize().height, 64);
    }

    TEST_F(TextureUploadTest, MipmapUploadPreservesBaseDimensions)
    {
        texture.putImage(0, GL_RGBA, 256, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        texture.putImage(1, GL_RGBA, 128, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        EXPECT_EQ(texture.getSize().width, 256);
        EXPECT_EQ(texture.getSize().height, 128);
    }
} // namespace
