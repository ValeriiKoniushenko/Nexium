// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace NX
{
    // !!! Don't forget to update TagInfo kTagTable !!!
    enum Tag : std::uint32_t // NOLINT(*-use-enum-class)
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

} // namespace NX

//-----------------------------------------------------------------------------
// Bitwise operators
//-----------------------------------------------------------------------------
constexpr NX::Tag operator|(NX::Tag lhs, NX::Tag rhs)
{
    return static_cast<NX::Tag>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr NX::Tag operator&(NX::Tag lhs, NX::Tag rhs)
{
    return static_cast<NX::Tag>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr NX::Tag operator^(NX::Tag lhs, NX::Tag rhs)
{
    return static_cast<NX::Tag>(static_cast<uint32_t>(lhs) ^ static_cast<uint32_t>(rhs));
}

constexpr NX::Tag operator~(NX::Tag tag)
{
    return static_cast<NX::Tag>(~static_cast<uint32_t>(tag));
}

constexpr NX::Tag& operator|=(NX::Tag& lhs, NX::Tag rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

constexpr NX::Tag& operator&=(NX::Tag& lhs, NX::Tag rhs)
{
    lhs = lhs & rhs;
    return lhs;
}
