// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "glm/glm.hpp"
#include "nlohmann/json.hpp"

namespace SW
{

    class JsonAdapter
    {
    public:
        virtual ~JsonAdapter() = default;

        [[nodiscard]] virtual nlohmann::json toJson() const = 0;
        virtual void fromJson(const nlohmann::json& json) = 0;
    };

} // namespace SW

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
