/*
 * MIT License
 *
 * Copyright (c) 2018-2026 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "Core/Size.h"
#include "Entities/Actor.h"

namespace Core
{
    class BaseCamera : public Actor
    {
        ECS_COMPONENT_DECL(BaseCamera, Actor);

    public:
        constexpr static float minFov = 5.f;
        constexpr static float maxFov = 175.f;

    public:
        ~BaseCamera() override = default;

        [[nodiscard]] const glm::mat4& getMatrix();

        [[nodiscard]] const glm::mat4& getCachedProjectionMatrix() { return _cachedProjMatrix; }

        void lookAt(const glm::vec3& targetPosition);

        void setFrameSize(FSize2 size) noexcept;

        [[nodiscard]] FSize2 getFrameSize() const noexcept { return _frameSize; }

        void setFov(float fov) noexcept;

        [[nodiscard]] float getFov() const noexcept { return _fov; }

        void setNear(float value) noexcept;

        [[nodiscard]] float getNear() const noexcept { return _near; }

        void setFar(float value) noexcept;

        [[nodiscard]] float getFar() const noexcept { return _far; }

        void yaw(float y);

        void pitch(float x);

        void yawAndPitch(glm::vec2 xy);

        [[nodiscard]] StringAtom getCacheHash() const override;

        [[nodiscard]] FSize2 getOutputFrameSize();

        [[nodiscard]] glm::vec3 putMouseRay(float length);

    protected:
        void recalculateCameraMatrices();

        void tryToRecalculateCameraMatrices();

    protected:
        glm::mat4 _cachedProjMatrix = glm::mat4(1.f);
        glm::mat4 _cachedCalculatedMatrix = glm::mat4(1.f);
        FSize2 _frameSize = FSize2{ 600, 600 };
        float _fov = 75.f;
        float _far = 10'000.f;
        float _near = 0.1f;
        bool _isDirtyProjMatrix = true;
    };
} // namespace Core
