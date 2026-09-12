// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Position.h"
#include "JustReflectMe/Adapter.h"

namespace NX
{
    using GPos3 = Core::GlobalPosition3F;
    using GPos2 = Core::GlobalPosition2F;
    using RPos3 = Core::RelativePosition3F;
    using RPos2 = Core::RelativePosition2F;

    CLASS();
    class Transformable
    {
        R_FRIEND(Transformable);

    public:
        Transformable() = default;
        virtual ~Transformable() = default;
        Transformable(const Transformable&) = default;
        Transformable(Transformable&&) noexcept = default;
        Transformable& operator=(const Transformable&) = default;
        Transformable& operator=(Transformable&&) noexcept = default;

        [[nodiscard]] GPos3 getPosition() const noexcept;

        void setPosition(const GPos3& position) noexcept;

        void addPosition(const GPos3& position) noexcept;

        void moveForward(float offset) noexcept;

        void moveRight(float offset) noexcept;

        void moveUp(float offset) noexcept;

        void setRotation(const glm::vec3& value) noexcept;

        void rotate(const glm::vec3& value) noexcept;

        [[nodiscard]] const glm::vec3& getRotation() const noexcept;

        void setRotationX(float x) noexcept;

        void rotateX(float x) noexcept;

        [[nodiscard]] float getRotationX() const noexcept;

        void setRotationY(float y) noexcept;

        void rotateY(float y) noexcept;

        [[nodiscard]] float getRotationY() const noexcept;

        void setRotationZ(float z) noexcept;

        void rotateZ(float z) noexcept;

        [[nodiscard]] float getRotationZ() const noexcept;

        void setOrigin(const glm::vec3& origin) noexcept;

        void addOrigin(const glm::vec3& origin) noexcept;

        [[nodiscard]] const glm::vec3& getOrigin() const noexcept;

        void scale(const glm::vec3& value) noexcept;

        void setScale(const glm::vec3& value) noexcept;

        void addScale(const glm::vec3& value) noexcept;

        [[nodiscard]] glm::vec3 getScale() const noexcept;

        [[nodiscard]] glm::vec3 getForwardVector() const noexcept;

        [[nodiscard]] glm::vec3 getUpVector() const noexcept;

        [[nodiscard]] glm::vec3 getRightVector() const noexcept;

        virtual void recalculateMatrices(const glm::mat4& mat = glm::mat4(1.f));

        void tryToRecalculateMatrices(const glm::mat4& mat = glm::mat4(1.f));

        [[nodiscard]] glm::mat4& getModelMatrix() noexcept { return _cachedModelMatrix; }
        void setDirtyMatrices(bool v = true) noexcept { _isDirtyModelMatrix = v; }
        [[nodiscard]] bool hasDirtyMatrices() noexcept { return _isDirtyModelMatrix; }

        void yaw(float y);

        void pitch(float x);

        void yawAndPitch(glm::vec2 xy);

        void setTransformations(const Transformable& trans);

        friend void swap(Transformable& a, Transformable& b) noexcept
        {
            std::swap(a._cachedModelMatrix, b._cachedModelMatrix);
            std::swap(a._scale, b._scale);
            std::swap(a._origin, b._origin);
            std::swap(a._position, b._position);
            std::swap(a._rotation, b._rotation);
            std::swap(a._isDirtyModelMatrix, b._isDirtyModelMatrix);
        }

    protected:
        virtual void onDirtyMatrix() {}
        void onPostDeserialize(Transformable* obj, const RLogsCollector& logs);

    protected:
        glm::mat4 _cachedModelMatrix = glm::mat4(1.f);

        FIELD();
        glm::vec3 _scale = glm::vec3(1.f);

        FIELD();
        glm::vec3 _origin = glm::vec3(0.f);

        FIELD();
        GPos3 _position = NX::GPos3(0.f);

        FIELD();
        glm::vec3 _rotation = glm::vec3(0.f);

        bool _isDirtyModelMatrix = true;
    };
} // namespace NX

#include "Transformable.generated.h" // added by the code generator. Better don't move it.
