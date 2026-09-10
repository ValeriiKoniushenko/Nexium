/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "Core/Color.h"
#include "Core/Position.h"
#include "Core/Size.h"
#include "JustReflectMe/Adapter.h"
#include "nlohmann/json.hpp"

#include <optional>

#define R_FRIEND_DECL(Class, ...)                                                                  \
    R_FRIEND(Class, __VA_ARGS__);                                                                  \
                                                                                                   \
public:                                                                                            \
    [[nodiscard]] nlohmann::json serialize() const override;                                       \
    void deserialize(RResourceStream<RJsonResourceStream>& stream) override

#define R_FRIEND_IMPL(TypeName)                                                                    \
    nlohmann::json TypeName::serialize() const                                                     \
    {                                                                                              \
        return R<TypeName>::Serialize<RJsonResourceStream>(*this).getData();                       \
    }                                                                                              \
    void TypeName::deserialize(RResourceStream<RJsonResourceStream>& stream)                       \
    {                                                                                              \
        R<TypeName>::Deserialize(stream, *this);                                                   \
    }

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

    void to_json(nlohmann::json& j, const Color3& color);

    void to_json(nlohmann::json& j, const Color4& color);

    void to_json(nlohmann::json& j, const NormColor3& color);

    void to_json(nlohmann::json& j, const NormColor4& color);

    template<glm::length_t L, typename T, glm::qualifier Q = glm::defaultp>
    void to_json(nlohmann::json& j, const GlobalPosition<L, T, Q>& v)
    {
        auto out = nlohmann::json::array();
        // clang-format off
        if constexpr (L > 0) out.push_back(v.x);
        if constexpr (L > 1) out.push_back(v.y);
        if constexpr (L > 2) out.push_back(v.z);
        if constexpr (L > 3) out.push_back(v.w);
        // clang-format on

        if constexpr (L > 4)
        {
            static_assert(false, "Can't convert to json unrecognized GlobalPosition length");
        }

        j = out;
    }

    void from_json(const nlohmann::json& j, Color3& color);

    void from_json(const nlohmann::json& j, Color4& color);

    void from_json(const nlohmann::json& j, NormColor3& color);

    void from_json(const nlohmann::json& j, NormColor4& color);

    template<glm::length_t L, typename T, glm::qualifier Q = glm::defaultp>
    void from_json(const nlohmann::json& j, GlobalPosition<L, T, Q>& v)
    {
        // clang-format off
        if constexpr (L > 0) j.at(0).get_to(v.x);
        if constexpr (L > 1) j.at(1).get_to(v.y);
        if constexpr (L > 2) j.at(2).get_to(v.z);
        if constexpr (L > 3) j.at(3).get_to(v.w);
        // clang-format on

        if constexpr (L > 4)
        {
            static_assert(false, "Can't convert from json unrecognized GlobalPosition length");
        }
    }
} // namespace Core

template<class T>
void to_json(nlohmann::json& j, const std::optional<T>& opt)
{
    if (opt.has_value())
    {
        j = opt.value();
    }
    else
    {
        j = nullptr;
    }
}

template<class T>
void from_json(const nlohmann::json& j, std::optional<T>& opt)
{
    if (j.is_null())
    {
        opt = std::nullopt;
    }
    else
    {
        opt = j.get<T>();
    }
}
