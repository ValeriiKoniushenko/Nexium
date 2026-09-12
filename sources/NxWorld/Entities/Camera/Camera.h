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
#include "Entities/Actor.h"

namespace NX
{
    ENUM_CLASS();
    enum class CameraType : uint8_t
    {
        Perspective,
        Orthographic,
    };

    CLASS();
    class BaseCamera : public Actor
    {
        ECS_DECL(BaseCamera, Core::Actor);

    public:
        BaseCamera(const BaseCamera&) = default;
        BaseCamera(BaseCamera&&) noexcept = default;
        BaseCamera& operator=(const BaseCamera&) = default;
        BaseCamera& operator=(BaseCamera&&) noexcept = default;
        ~BaseCamera() override;

        [[nodiscard]] virtual const glm::mat4& getMatrix() = 0;
        [[nodiscard]] virtual glm::vec3 putMouseRay(float length) = 0;
        [[nodiscard]] virtual CameraType getType() const noexcept = 0;

        [[nodiscard]] const glm::mat4& getCachedProjectionMatrix() { return _cachedProjMatrix; }

        [[nodiscard]] FSize2 getOutputFrameSize();

        [[nodiscard]] glm::vec3 getGlobalPosition() const override { return _worldPos; }
        [[nodiscard]] glm::vec3 getGlobalRotation() const override { return _worldRotation; }

        void setNear(float value) noexcept;
        [[nodiscard]] float getNear() const noexcept { return _near; }

        void setFar(float value) noexcept;
        [[nodiscard]] float getFar() const noexcept { return _far; }

        [[nodiscard]] Core::StringAtom getCacheHash() const override;

        void tryToRecalculateCameraMatrices();
        void invalidateCameraMatrices();

        [[nodiscard]] nlohmann::json getTypeSpecificSceneDataAsJson() const override;
        void applyTypeSpecificSceneData(const nlohmann::json& data) override;

    protected:
        void recalculateCameraMatrices();

    protected:
        glm::mat4 _cachedProjMatrix = glm::mat4(1.f);
        glm::mat4 _cachedCalculatedMatrix = glm::mat4(1.f);
        glm::vec3 _worldRotation = glm::vec3(0.f);
        glm::vec3 _worldPos = glm::vec3(0.f);

        FIELD();
        float _far = 10'000.f;
        FIELD();
        float _near = 0.1f;

        bool _isDirtyProjMatrix = true;
    };

    CLASS();
    class OrthographicCamera : public BaseCamera
    {
        ECS_DECL(OrthographicCamera, Core::BaseCamera);

    public:
        OrthographicCamera(const OrthographicCamera&) = default;
        OrthographicCamera(OrthographicCamera&&) noexcept = default;
        OrthographicCamera& operator=(const OrthographicCamera&) = default;
        OrthographicCamera& operator=(OrthographicCamera&&) noexcept = default;
        ~OrthographicCamera() override = default;

        [[nodiscard]] const glm::mat4& getMatrix() override;
        [[nodiscard]] glm::vec3 putMouseRay(float length) override;

        [[nodiscard]] CameraType getType() const noexcept override
        {
            return CameraType::Orthographic;
        }

        [[nodiscard]] nlohmann::json getTypeSpecificSceneDataAsJson() const override;
        void applyTypeSpecificSceneData(const nlohmann::json& data) override;

    protected:
        FIELD();
        float _zoom = 1.f;
    };

    CLASS();
    class PerspectiveCamera : public BaseCamera
    {
        ECS_DECL(PerspectiveCamera, Core::BaseCamera);

    public:
        constexpr static float minFov = 5.f;
        constexpr static float maxFov = 175.f;

    public:
        PerspectiveCamera(const PerspectiveCamera&) = default;
        PerspectiveCamera(PerspectiveCamera&&) noexcept = default;
        PerspectiveCamera& operator=(const PerspectiveCamera&) = default;
        PerspectiveCamera& operator=(PerspectiveCamera&&) noexcept = default;
        ~PerspectiveCamera() override = default;

        [[nodiscard]] const glm::mat4& getMatrix() override;

        void lookAt(const glm::vec3& targetPosition);

        void setAspect(FSize2 size) noexcept;
        [[nodiscard]] FSize2 getAspect() const noexcept { return _aspect; }

        void setFov(float fov) noexcept;
        [[nodiscard]] float getFov() const noexcept { return _fov; }

        [[nodiscard]] glm::vec3 putMouseRay(float length) override;

        [[nodiscard]] CameraType getType() const noexcept override
        {
            return CameraType::Perspective;
        }

    protected:
        FIELD();
        Core::FSize2 _aspect = Core::FSize2{ 1.f, 1.f };
        FIELD();
        float _fov = 75.f;
    };

} // namespace NX

#include "Camera.generated.h" // added by the code generator. Better don't move it.
