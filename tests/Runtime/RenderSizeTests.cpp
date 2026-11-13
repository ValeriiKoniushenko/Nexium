// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Graphics/GLContextTest.h"
#include "NxWorld/Framework/GameInstance.h"
#include "Platform/Input/Window.h"

#include <array>

namespace
{
    class RenderSizeTests : public GLContextTest
    {
    };
} // namespace

TEST_F(RenderSizeTests, GameOnlyRenderSizeMatchesTheFramebufferTheViewportUses)
{
    // In game-only mode the orthographic projection is built from
    // GameInstance::getRenderSize() while glViewport() uses the window's framebuffer size.
    // On a HiDPI / scaled desktop (Wayland fractional scaling, macOS Retina) GLFW reports a
    // framebuffer larger than the logical window; if the two sources disagree the world is
    // drawn at the content scale and picking/mouse math drifts. The two must be the same size
    // (they are on an unscaled desktop, which is why this passes on most machines).
    auto& window = Platform::GetWindow();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    window.create("Nexium_Tests RenderSize"_atom, Core::ISize2{ 320, 240 });

    auto executableName = std::to_array("Nexium_Tests");
    std::array<char*, 1> arguments{ executableName.data() };
    NX::GameInstance gameInstance{ 1, arguments.data() };
    gameInstance.window = &window;
    gameInstance.renderMode = NX::GameInstance::RenderMode::GameOnly;

    const auto renderSize = gameInstance.getRenderSize();
    const auto framebufferSize = window.getFramebufferSize();
    EXPECT_EQ(renderSize.width, framebufferSize.width);
    EXPECT_EQ(renderSize.height, framebufferSize.height);

    window.destroy();
}
