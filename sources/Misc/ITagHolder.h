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

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace Core
{
    // !!! Don't forget to update TagInfo kTagTable !!!

    enum Tag : std::uint32_t
    {
        Tag_None = 0,
        Tag_WorldObject = 1 << 0,
        Tag_GuiObject = 1 << 1,
        Tag_EditorInternal = 1 << 2,
        Tag_AnimationController = 1 << 3,
        Tag_Any = 0xFFFFFFFF
    };

    struct TagHelper
    {
        TagHelper() = delete;

        [[nodiscard]] static std::string JoinAllToString(Tag tags, char div = ',');
        [[nodiscard]] static std::string ToString(Tag tag);
        [[nodiscard]] static std::optional<Tag> ToTag(std::string_view tagStr);
        [[nodiscard]] static std::vector<std::string> ToStrings(uint32_t tags);
        [[nodiscard]] static std::string StringifyToStrings(uint32_t tags, const char* div = ", ");
        [[nodiscard]] static Tag SplitToTagFromString(std::string_view str, char div = ',');
    };

    class ITagHolder
    {
    public:
        ITagHolder(const ITagHolder&) = default;
        ITagHolder& operator=(const ITagHolder&) = default;
        ITagHolder(ITagHolder&&) noexcept = default;
        ITagHolder& operator=(ITagHolder&&) noexcept = default;
        virtual ~ITagHolder() = default;

        [[nodiscard]] virtual Tag getTags() const { return Tag_None; }

    protected:
        ITagHolder() = default;
    };

} // namespace Core

//-----------------------------------------------------------------------------
// Bitwise operators
//-----------------------------------------------------------------------------
constexpr Core::Tag operator|(Core::Tag lhs, Core::Tag rhs)
{
    return static_cast<Core::Tag>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr Core::Tag operator&(Core::Tag lhs, Core::Tag rhs)
{
    return static_cast<Core::Tag>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr Core::Tag operator^(Core::Tag lhs, Core::Tag rhs)
{
    return static_cast<Core::Tag>(static_cast<uint32_t>(lhs) ^ static_cast<uint32_t>(rhs));
}

constexpr Core::Tag operator~(Core::Tag tag)
{
    return static_cast<Core::Tag>(~static_cast<uint32_t>(tag));
}

constexpr Core::Tag& operator|=(Core::Tag& lhs, Core::Tag rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

constexpr Core::Tag& operator&=(Core::Tag& lhs, Core::Tag rhs)
{
    lhs = lhs & rhs;
    return lhs;
}
