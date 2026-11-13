// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Platform/Graphics/Graphics.h"
#include "Platform/Input/Glfw.h"

#include "gtest/gtest.h"

/// Fixture for tests that need real GL objects. Each test gets a hidden GLFW window with an
/// OpenGL 4.5 core context, torn down again afterwards so the rest of the suite keeps running
/// with GLFW uninitialised (engine code polls input through Platform::Window, which has no
/// window in unit tests). Where no display or driver is available (CI unit-test jobs run
/// headless) every test on the fixture is skipped, so GL-level checks can live next to the
/// logic tests without making the suite depend on a GPU.
class GLContextTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        if (!glfwInit())
        {
            GTEST_SKIP() << "GLFW cannot initialise in this environment.";
        }
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        _window = glfwCreateWindow(64, 64, "Nexium_Tests GL", nullptr, nullptr);
        if (!_window)
        {
            glfwTerminate();
            GTEST_SKIP() << "No OpenGL 4.5 context is available in this environment.";
        }
        glfwMakeContextCurrent(_window);
        if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0)
        {
            TearDown();
            GTEST_SKIP() << "GL function loading failed.";
        }
    }

    void TearDown() override
    {
        if (_window)
        {
            glfwDestroyWindow(_window);
            _window = nullptr;
            glfwTerminate();
        }
    }

private:
    GLFWwindow* _window = nullptr;
};
