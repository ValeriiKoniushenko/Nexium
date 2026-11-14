// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Architecture gap in the BaseGraphicsData family for anything that is not an indexed
// triangle mesh (spline/polyline rendering, trails, particle streams, procedural animation
// that re-uploads vertices every frame). Expected to FAIL until the class can draw a plain
// vertex stream.

#include "Foundation/Configs.h"
#include "Graphics/GLContextTest.h"
#include "NxSubsystems/Graphics/GraphicsComponents.h"
#include "NxSubsystems/Graphics/ShaderManager.h"
#include "NxSubsystems/Graphics/ShaderProgram.h"

#include <array>
#include <filesystem>
#include <vector>

namespace
{
    constexpr int kFrame = 64;

    class GraphicsDataDrawGapsTests : public GLContextTest
    {
    protected:
        void SetUp() override
        {
            GLContextTest::SetUp();
            if (IsSkipped())
            {
                return;
            }
            NX::GetShaderManager().loadShaders(Foundation::Config::Path::shaders);
            _line = NX::GetShaderManager().getShaderProgram("line"_atom);
            ASSERT_NE(_line, nullptr);
            // Same vertex layout GameInstance::initializeShaders installs for "line".
            _line->setSetEventCallback(
                [](NX::ShaderProgram::Event event)
                {
                    if ((event & NX::ShaderProgram::Event::OnSetVertexBuffer) != 0)
                    {
                        glEnableVertexAttribArray(0);
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
                    }
                });
            glViewport(0, 0, kFrame, kFrame);
        }

        void TearDown() override
        {
            const auto emptyDir = std::filesystem::temp_directory_path() / "nexium-no-shaders";
            std::filesystem::create_directories(emptyDir);
            NX::GetShaderManager().loadShaders(emptyDir); // frees programs with a live context
            GLContextTest::TearDown();
        }

        [[nodiscard]] static int countNonClearPixels()
        {
            std::array<unsigned char, kFrame * kFrame * 4> pixels{};
            glFinish();
            glReadBuffer(GL_BACK);
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glReadPixels(0, 0, kFrame, kFrame, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
            int count = 0;
            for (std::size_t i = 0; i < pixels.size(); i += 4)
            {
                count += (pixels[i] != 0 || pixels[i + 1] != 0 || pixels[i + 2] != 0) ? 1 : 0;
            }
            return count;
        }

        NX::ShaderProgram* _line = nullptr;
    };
} // namespace

TEST_F(GraphicsDataDrawGapsTests, AVertexStreamWithoutIndicesIsDrawable)
{
    // A spline is a polyline: N vertices drawn as GL_LINE_STRIP, re-uploaded as the curve
    // animates. BaseGraphicsData only counts elements in setIndexBuffer() and directDraw()
    // always issues glDrawElements, so a vertex-only stream draws nothing (count 0) and the
    // caller has to fabricate an identity index buffer or bypass the class entirely, which is
    // what Face::drawText already does with a raw glDrawArrays.
    const std::vector<float> clipSpaceZigZag = {
        -0.9f, -0.9f, 0.f, //
        0.9f,  -0.9f, 0.f, //
        0.9f,  0.9f,  0.f, //
        -0.9f, 0.9f,  0.f, //
    };

    NX::BaseGraphicsData polyline;
    polyline.generate();
    polyline.setShader(_line);
    polyline.setVertexBuffer(clipSpaceZigZag, GL_DYNAMIC_DRAW);
    ASSERT_TRUE(polyline.isValid());

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    _line->use();
    _line->setUniform("uProjAndView"_atom, glm::mat4(1.f));
    _line->setUniform("uColor"_atom, Core::NormColor4(1.f, 1.f, 0.f, 1.f));
    polyline.directDraw(GL_LINE_STRIP);

    EXPECT_GT(countNonClearPixels(), 0) << "vertex-only geometry is never drawn";
    polyline.clear();
}
