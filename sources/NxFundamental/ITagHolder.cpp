// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "ITagHolder.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <sstream>
#include <string_view>

namespace
{
    struct TagInfo
    {
        NX::Tag tag;
        std::string_view name;
    };

    // clang-format off
    constexpr std::array<TagInfo, 5> kTagTable =
    {{
        { .tag = NX::Tag_None,             .name = "None" },
        { .tag = NX::Tag_WorldObject,      .name = "WorldObject" },
        { .tag = NX::Tag_GuiObject,        .name = "GuiObject" },
        { .tag = NX::Tag_EditorInternal,   .name = "EditorInternal" },
        { .tag = NX::Tag_AnimationController, .name = "AnimationController" },
    }};
    // clang-format on

} // namespace

namespace NX
{

    std::string TagHelper::JoinAllToString(Tag tags, char div)
    {
        const auto value = static_cast<uint32_t>(tags);

        if (value == 0) [[unlikely]]
        {
            return "None";
        }

        std::string result;

        for (const auto& info : kTagTable)
        {
            if (info.tag == Tag_None)
            {
                continue;
            }

            if ((value & static_cast<uint32_t>(info.tag)) == 0)
            {
                continue;
            }

            if (!result.empty())
            {
                result += div;
            }

            result += info.name;
        }

        return result;
    }

    std::string TagHelper::ToString(Tag tag)
    {
        for (const auto& info : kTagTable)
        {
            if (info.tag == tag)
            {
                return std::string(info.name);
            }
        }

        return "";
    }

    std::optional<Tag> TagHelper::ToTag(std::string_view tagStr)
    {
        for (const auto& info : kTagTable)
        {
            if (info.name == tagStr)
            {
                return info.tag;
            }
        }

        return std::nullopt;
    }

    std::vector<std::string> TagHelper::ToStrings(uint32_t tags)
    {
        std::vector<std::string> result;

        if (tags == 0)
        {
            result.emplace_back("None");
            return result;
        }

        for (const auto& info : kTagTable)
        {
            if (info.tag == Tag_None)
            {
                continue;
            }

            if (tags & static_cast<uint32_t>(info.tag))
            {
                result.emplace_back(info.name);
            }
        }

        return result;
    }

    std::string TagHelper::StringifyToStrings(uint32_t tags, const char* div)
    {
        std::string out;
        for (auto&& str : TagHelper::ToStrings(tags))
        {
            out += str + div;
        }

        if (!out.empty())
        {
            const auto len = strlen(div);
            out.erase(out.size() - len, len);
        }

        return out;
    }

    Tag TagHelper::SplitToTagFromString(std::string_view str, char div)
    {
        Tag result = Tag_None;

        size_t begin = 0;

        while (begin < str.size())
        {
            const size_t end = str.find(div, begin);

            std::string_view token = (end == std::string_view::npos)
                                         ? str.substr(begin)
                                         : str.substr(begin, end - begin);

            // Trim leading/trailing spaces.
            while (!token.empty() && token.front() == ' ')
            {
                token.remove_prefix(1);
            }

            while (!token.empty() && token.back() == ' ')
            {
                token.remove_suffix(1);
            }

            if (auto tag = ToTag(token))
            {
                result |= *tag;
            }

            if (end == std::string_view::npos)
            {
                break;
            }

            begin = end + 1;
        }

        return result;
    }

} // namespace NX
