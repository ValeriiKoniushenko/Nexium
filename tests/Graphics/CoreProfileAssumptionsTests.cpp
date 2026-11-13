// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Graphics/GLContextTest.h"

namespace
{
    class CoreProfileAssumptionsTests : public GLContextTest
    {
    };
} // namespace

TEST_F(CoreProfileAssumptionsTests, WideLinesAreAcceptedByTheEngineContext)
{
    // Debug::Line::Draw (selection outlines) relies on glLineWidth(2.f). Wide lines are
    // deprecated in core profiles and some drivers reject widths > 1 with GL_INVALID_VALUE
    // (forward-compatible contexts). The engine creates a plain 4.x core context like this
    // fixture, so this pins down that the outline thickness really is honoured here; a failure
    // on another driver means the outline must be drawn as quads instead.
    while (glGetError() != GL_NO_ERROR)
    {
    }

    glLineWidth(2.f);
    const GLenum error = glGetError();

    EXPECT_EQ(error, static_cast<GLenum>(GL_NO_ERROR));
    GLfloat range[2] = { 0.f, 0.f };
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, range);
    EXPECT_GE(range[1], 2.f) << "driver cannot rasterise 2px lines";
    glLineWidth(1.f);
}
