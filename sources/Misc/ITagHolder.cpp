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

#include "ITagHolder.h"

#include <algorithm>
#include <sstream>
#include <string_view>

namespace
{
    struct TagInfo
    {
        Core::Tag tag;
        std::string_view name;
    };

    // clang-format off
     constexpr TagInfo kTagTable[] =
    {
        { Core::Tag_None,        "None" },
        { Core::Tag_WorldObject, "WorldObject" },
        { Core::Tag_GuiObject,   "GuiObject" },
        { Core::Tag_EditorInternal,   "EditorInternal" },
        };
    // clang-format on

} // namespace

namespace Core
{

    std::string TagHelper::JoinAllToString(Tag tags, char div)
    {
        const uint32_t value = static_cast<uint32_t>(tags);

        if (value == 0)
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

} // namespace Core
