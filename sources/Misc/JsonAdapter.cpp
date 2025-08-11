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

#include "JsonAdapter.h"

namespace glm
{
    void to_json(nlohmann::json& j, const vec4& v)
    {
        j = { v.x, v.y, v.z, v.w };
    }

    void from_json(const nlohmann::json& j, vec4& v)
    {
        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
        v.w = j.at(3).get<float>();
    }

    void to_json(nlohmann::json& j, const vec3& v)
    {
        j = { v.x, v.y, v.z };
    }
    void from_json(const nlohmann::json& j, vec3& v)
    {
        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
    }

    void to_json(nlohmann::json& j, const vec2& v)
    {
        j = { v.x, v.y };
    }
    void from_json(const nlohmann::json& j, vec2& v)
    {
        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
    }

    void to_json(nlohmann::json& j, const ivec4& v)
    {
        j = { v.x, v.y, v.z, v.w };
    }
    void from_json(const nlohmann::json& j, ivec4& v)
    {
        v.x = j.at(0).get<int>();
        v.y = j.at(1).get<int>();
        v.z = j.at(2).get<int>();
        v.w = j.at(3).get<int>();
    }

    void to_json(nlohmann::json& j, const ivec3& v)
    {
        j = { v.x, v.y, v.z };
    }
    void from_json(const nlohmann::json& j, ivec3& v)
    {
        v.x = j.at(0).get<int>();
        v.y = j.at(1).get<int>();
        v.z = j.at(2).get<int>();
    }

    void to_json(nlohmann::json& j, const ivec2& v)
    {
        j = { v.x, v.y };
    }

    void from_json(const nlohmann::json& j, ivec2& v)
    {
        v.x = j.at(0).get<int>();
        v.y = j.at(1).get<int>();
    }
} // namespace glm

namespace Core
{
    void to_json(nlohmann::json& j, FSize2 value)
    {
        j["width"] = value.width;
        j["height"] = value.height;
    }

    void to_json(nlohmann::json& j, const FSize3& value)
    {
        j["width"] = value.width;
        j["height"] = value.height;
        j["deep"] = value.deep;
    }

    void to_json(nlohmann::json& j, ISize2 value)
    {
        j["width"] = value.width;
        j["height"] = value.height;
    }

    void to_json(nlohmann::json& j, const ISize3& value)
    {
        j["width"] = value.width;
        j["height"] = value.height;
        j["deep"] = value.deep;
    }

    void from_json(const nlohmann::json& j, FSize2& value)
    {
        if (j.contains("width"))
        {
            value.width = j["width"].get<float>();
        }
        if (j.contains("height"))
        {
            value.height = j["height"].get<float>();
        }
    }

    void from_json(const nlohmann::json& j, FSize3& value)
    {
        if (j.contains("width"))
        {
            value.width = j["width"].get<float>();
        }
        if (j.contains("height"))
        {
            value.height = j["height"].get<float>();
        }
        if (j.contains("deep"))
        {
            value.deep = j["deep"].get<float>();
        }
    }

    void from_json(const nlohmann::json& j, ISize2& value)
    {
        if (j.contains("width"))
        {
            value.width = j["width"].get<int>();
        }
        if (j.contains("height"))
        {
            value.height = j["height"].get<int>();
        }
    }

    void from_json(const nlohmann::json& j, ISize3& value)
    {
        if (j.contains("width"))
        {
            value.width = j["width"].get<int>();
        }
        if (j.contains("height"))
        {
            value.height = j["height"].get<int>();
        }
        if (j.contains("deep"))
        {
            value.deep = j["deep"].get<int>();
        }
    }

} // namespace Core