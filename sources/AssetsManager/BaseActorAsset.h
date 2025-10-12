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

#include "BaseAsset.h"
#include "GameplaySystem/Framework/Actor.h"

namespace Core
{

    template<IsActorBased T>
    class BaseActorAsset : public BaseAsset
    {
    public:
        explicit BaseActorAsset(const StringAtom& logicPath)
            : BaseAsset(logicPath)
        {
        }

        ~BaseActorAsset() override = default;

        [[nodiscard]] const T& getData() const noexcept { return _data; }
        [[nodiscard]] T& getData() noexcept { return _data; }

        [[nodiscard]] bool isPostDraw() const noexcept { return _isPostDraw; }
        void setIsPostDraw(bool value) noexcept { _isPostDraw = value; }

        [[nodiscard]] const glm::vec3& getScale() const noexcept { return _scale; }
        void setScale(const glm::vec3& value) noexcept { _scale = value; }

        [[nodiscard]] const glm::vec3& getOrigin() const noexcept { return _origin; }
        void setOrigin(const glm::vec3& value) noexcept { _origin = value; }

        [[nodiscard]] const GPos3& getPosition() const noexcept { return _position; }
        void setPosition(const GPos3& value) noexcept { _position = value; }

        [[nodiscard]] const glm::vec3& getRotation() const noexcept { return _rotation; }
        void setRotation(const glm::vec3& value) noexcept { _rotation = value; }

    protected:
        void ioFieldsUpdate(DataStream& stream) override
        {
            // Actor's properties
            stream.field("isPostDraw", _isPostDraw);

            // Transformable's properties
            stream.field("scale", _scale, glm::vec3(1.f, 1.f, 1.f));
            stream.field("origin", _origin);
            stream.field("position", _position);
            stream.field("rotation", _rotation);
        }

        void applyAssetSettingsToObject()
        {
            // Actor's properties
            _data.setIsPostDraw(_isPostDraw);

            // Transformable's properties
            _data.setScale(_scale);
            _data.setOrigin(_origin);
            _data.setPosition(_position);
            _data.setRotation(_rotation);
        }

    protected:
        T _data;

        // Actor's properties
        bool _isPostDraw = false;

        // Transformable's properties
        glm::vec3 _scale{};
        glm::vec3 _origin{};
        GPos3 _position{};
        glm::vec3 _rotation{};
    };

} // namespace Core