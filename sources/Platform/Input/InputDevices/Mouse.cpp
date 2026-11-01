// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Mouse.h"

#include "Window.h"

namespace Platform
{

    glm::vec2 Mouse::GetPosition()
    {
        double x = 0;
        double y = 0;
        glfwGetCursorPos(GetWindow().getRawWindow(), &x, &y);
        return { static_cast<float>(x), static_cast<float>(y) };
    }

    glm::vec2 Mouse::GetInViewportPosition()
    {
        return GetPosition();
    }

    bool Mouse::IsKeyPressed(Key key)
    {
        return glfwGetMouseButton(GetWindow().getRawWindow(), static_cast<int>(key)) == GLFW_PRESS;
    }

    bool Mouse::IsKeyReleased(Key key)
    {
        return glfwGetMouseButton(GetWindow().getRawWindow(), static_cast<int>(key))
               == GLFW_RELEASE;
    }

    bool Mouse::isKeyRepeated(Key key)
    {
        return glfwGetMouseButton(GetWindow().getRawWindow(), static_cast<int>(key)) == GLFW_REPEAT;
    }
} // namespace Platform
