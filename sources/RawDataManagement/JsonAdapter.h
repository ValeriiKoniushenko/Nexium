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
#include "glm/glm.hpp"
#include "nlohmann/json.hpp"

#include <format>

namespace Core
{
    class JsonAdapter
    {
    public:
        struct Exception
        {
            explicit Exception(std::string msg)
                : message{ std::move(msg) }
            {
            }

            std::string message;
        };

    public:
        virtual ~JsonAdapter() = default;

        [[nodiscard]] virtual nlohmann::json toJson() const = 0;

        virtual void fromJson(const nlohmann::json& json, bool isIgnoreChildren) = 0;

        template<class T>
        [[nodiscard]] T requireAs(const char* key, const nlohmann::json& json)
        {
            if (!json.contains(key))
            {
                throw Exception(std::format("Can't read key: '{}'", key));
            }

            return json[key].get<T>();
        }

        template<class T>
        [[nodiscard]] T tryReadJsonAs(const char* key, const nlohmann::json& json)
        {
            if (json.contains(key))
            {
                return json[key].get<T>();
            }

            return {};
        }

        template<class T>
            requires(!std::derived_from<T, JsonAdapter>)
        void tryReadJsonTo(T& out, const char* key, const nlohmann::json& json)
        {
            if (json.contains(key))
            {
                out = json[key].get<T>();
            }
        }
    };
} // namespace Core

namespace glm
{
    void to_json(nlohmann::json& j, const vec4& v);

    void to_json(nlohmann::json& j, const vec3& v);

    void to_json(nlohmann::json& j, const vec2& v);

    void to_json(nlohmann::json& j, const ivec4& v);

    void to_json(nlohmann::json& j, const ivec3& v);

    void to_json(nlohmann::json& j, const ivec2& v);

    void from_json(const nlohmann::json& j, vec4& v);

    void from_json(const nlohmann::json& j, vec3& v);

    void from_json(const nlohmann::json& j, vec2& v);

    void from_json(const nlohmann::json& j, ivec4& v);

    void from_json(const nlohmann::json& j, ivec3& v);

    void from_json(const nlohmann::json& j, ivec2& v);
} // namespace glm

namespace Core
{

    void to_json(nlohmann::json& j, FSize2);

    void to_json(nlohmann::json& j, const FSize3&);

    void to_json(nlohmann::json& j, ISize2);

    void to_json(nlohmann::json& j, const ISize3&);

    void from_json(const nlohmann::json& j, FSize2& value);

    void from_json(const nlohmann::json& j, FSize3& value);

    void from_json(const nlohmann::json& j, ISize2& value);

    void from_json(const nlohmann::json& j, ISize3& value);
} // namespace Core
