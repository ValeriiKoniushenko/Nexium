// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Fundamental::ResourceManagement

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

    void to_json(nlohmann::json& j, Core::FSize2);

    void to_json(nlohmann::json& j, const Core::FSize3&);

    void to_json(nlohmann::json& j, Core::ISize2);

    void to_json(nlohmann::json& j, const Core::ISize3&);

    void from_json(const nlohmann::json& j, Core::FSize2& value);

    void from_json(const nlohmann::json& j, Core::FSize3& value);

    void from_json(const nlohmann::json& j, Core::ISize2& value);

    void from_json(const nlohmann::json& j, Core::ISize3& value);

    void to_json(nlohmann::json& j, const Core::Color3& color);

    void to_json(nlohmann::json& j, const Core::Color4& color);

    void to_json(nlohmann::json& j, const Core::NormColor3& color);

    void to_json(nlohmann::json& j, const Core::NormColor4& color);

    template<glm::length_t L, typename T, glm::qualifier Q = glm::defaultp>
    void to_json(nlohmann::json& j, const Core::GlobalPosition<L, T, Q>& v)
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

    void from_json(const nlohmann::json& j, Core::Color3& color);

    void from_json(const nlohmann::json& j, Core::Color4& color);

    void from_json(const nlohmann::json& j, Core::NormColor3& color);

    void from_json(const nlohmann::json& j, Core::NormColor4& color);

    template<glm::length_t L, typename T, glm::qualifier Q = glm::defaultp>
    void from_json(const nlohmann::json& j, Core::GlobalPosition<L, T, Q>& v)
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
