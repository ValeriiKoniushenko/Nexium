// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "ReflectionFixtures.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <algorithm>
#include <any>
#include <filesystem>
#include <string>
#include <vector>

using namespace Core;
using namespace ReflectionFixtures;
using NX::Reflection::Describe;
using NX::Reflection::TypeInfo;
using NX::Reflection::TypeInfoOf;
using NX::Reflection::TypeKind;
using testing::ElementsAre;

namespace
{
    /// Sorted, so hash-map key order does not matter.
    std::vector<std::string> Strings(const std::vector<StringAtom>& atoms)
    {
        std::vector<std::string> out;
        for (const auto& atom : atoms)
        {
            out.push_back(atom.toStdString());
        }
        std::ranges::sort(out);
        return out;
    }
} // namespace

// ===================== Values =====================

TEST(TypeInfo, ScalarsAndStringsAreValues)
{
    EXPECT_EQ(TypeInfoOf<float>().kind(), TypeKind::Value);
    EXPECT_EQ(TypeInfoOf<std::string>().kind(), TypeKind::Value) << "not a sequence of chars";
    EXPECT_EQ(TypeInfoOf<StringAtom>().kind(), TypeKind::Value);
    EXPECT_EQ(TypeInfoOf<std::filesystem::path>().kind(), TypeKind::Value);
    EXPECT_EQ(TypeInfoOf<float>().type(), std::type_index(typeid(float)));
    EXPECT_EQ(&TypeInfoOf<float>(), &TypeInfoOf<const float>());
}

TEST(TypeInfo, CopiesAndAssignsAValue)
{
    const TypeInfo& info = TypeInfoOf<std::string>();
    const std::string source = "hello";

    std::any box = info.copy(&source);
    auto* copy = static_cast<std::string*>(info.address(box));
    ASSERT_NE(copy, nullptr);
    EXPECT_EQ(*copy, "hello");

    std::string target;
    EXPECT_TRUE(info.assign(&target, &source));
    EXPECT_EQ(target, "hello");
}

TEST(TypeInfo, MoveOnlyContentIsNotCopyable)
{
    const TypeInfo& info = TypeInfoOf<std::vector<std::unique_ptr<int>>>();
    std::vector<std::unique_ptr<int>> source;
    std::vector<std::unique_ptr<int>> target;

    EXPECT_FALSE(info.isCopyable());
    EXPECT_FALSE(info.copy(&source).has_value());
    EXPECT_FALSE(info.assign(&target, &source));
}

// ===================== Enums =====================

TEST(TypeInfo, ReflectedEnumListsEntriesInDeclarationOrder)
{
    const TypeInfo& info = TypeInfoOf<Mode>();

    ASSERT_EQ(info.kind(), TypeKind::Enum);
    std::vector<std::string> names;
    std::vector<std::uint64_t> bits;
    for (const auto& entry : info.enumEntries())
    {
        names.push_back(entry.name.toStdString());
        bits.push_back(entry.bits);
    }
    EXPECT_THAT(names, ElementsAre("Off", "Low", "High"));
    EXPECT_THAT(bits, ElementsAre(0u, 1u, 4u));
}

TEST(TypeInfo, EnumBitsRoundTrip)
{
    const TypeInfo& info = TypeInfoOf<Mode>();
    Mode mode = Mode::Low;

    EXPECT_EQ(info.enumBits(&mode), 1u);
    info.setEnumBits(&mode, 4);
    EXPECT_EQ(mode, Mode::High);
}

// ===================== Classes =====================

TEST(TypeInfo, ReflectedStructIsAClass)
{
    const TypeInfo& info = TypeInfoOf<Point>();

    EXPECT_EQ(info.kind(), TypeKind::Class);
    EXPECT_EQ(info.classDesc(), &Describe<Point>());
    EXPECT_TRUE(info.isCopyable());
}

// ===================== Optional =====================

TEST(TypeInfo, OptionalCanBeSetAndReset)
{
    const TypeInfo& info = TypeInfoOf<std::optional<float>>();
    std::optional<float> value;

    ASSERT_EQ(info.kind(), TypeKind::Optional);
    EXPECT_EQ(info.element(), &TypeInfoOf<float>());
    EXPECT_FALSE(info.hasValue(&value));
    EXPECT_EQ(info.value(&value), nullptr);

    info.emplace(&value);
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(info.value(&value), &*value);

    info.reset(&value);
    EXPECT_FALSE(value.has_value());
}

// ===================== Sequences =====================

TEST(TypeInfo, VectorIsAResizableSequence)
{
    const TypeInfo& info = TypeInfoOf<std::vector<int>>();
    std::vector<int> values = { 1, 2, 3 };

    ASSERT_EQ(info.kind(), TypeKind::Sequence);
    EXPECT_FALSE(info.isFixedSize());
    EXPECT_EQ(info.element(), &TypeInfoOf<int>());
    EXPECT_EQ(info.size(&values), 3u);
    EXPECT_EQ(info.at(&values, 1), &values[1]);
    EXPECT_EQ(info.at(&values, 3), nullptr);

    info.append(&values);
    EXPECT_THAT(values, ElementsAre(1, 2, 3, 0));
    info.erase(&values, 0);
    EXPECT_THAT(values, ElementsAre(2, 3, 0));
    info.move(&values, 2, 0);
    EXPECT_THAT(values, ElementsAre(0, 2, 3));
    info.move(&values, 0, 1);
    EXPECT_THAT(values, ElementsAre(2, 0, 3));
}

TEST(TypeInfo, ArrayIsAFixedSequence)
{
    const TypeInfo& info = TypeInfoOf<std::array<int, 2>>();
    std::array<int, 2> pair = { 7, 8 };

    ASSERT_EQ(info.kind(), TypeKind::Sequence);
    EXPECT_TRUE(info.isFixedSize());
    EXPECT_EQ(info.size(&pair), 2u);
    info.append(&pair);
    info.erase(&pair, 0);
    EXPECT_THAT(pair, ElementsAre(7, 8));
    info.move(&pair, 0, 1);
    EXPECT_THAT(pair, ElementsAre(8, 7));
}

// ===================== Maps =====================

TEST(TypeInfo, StringKeyedMapIsEditableByKey)
{
    const TypeInfo& info = TypeInfoOf<std::unordered_map<StringAtom, int>>();
    std::unordered_map<StringAtom, int> named = { { "b"_atom, 2 }, { "a"_atom, 1 } };

    ASSERT_EQ(info.kind(), TypeKind::Map);
    EXPECT_EQ(info.element(), &TypeInfoOf<int>());
    EXPECT_THAT(Strings(info.keys(&named)), ElementsAre("a", "b"));
    EXPECT_EQ(info.find(&named, "a"_atom), &named.at("a"_atom));
    EXPECT_EQ(info.find(&named, "z"_atom), nullptr);

    EXPECT_TRUE(info.insert(&named, "c"_atom));
    EXPECT_FALSE(info.insert(&named, "c"_atom)) << "already there";
    EXPECT_EQ(named.at("c"_atom), 0);
    EXPECT_TRUE(info.eraseKey(&named, "a"_atom));
    EXPECT_FALSE(info.eraseKey(&named, "a"_atom));
    EXPECT_EQ(named.size(), 2u);
}

TEST(TypeInfo, StdStringKeysWork)
{
    const TypeInfo& info = TypeInfoOf<std::map<std::string, Point>>();
    std::map<std::string, Point> points;

    ASSERT_EQ(info.kind(), TypeKind::Map);
    EXPECT_EQ(info.element()->kind(), TypeKind::Class);
    EXPECT_TRUE(info.insert(&points, "home"_atom));
    EXPECT_THAT(Strings(info.keys(&points)), ElementsAre("home"));
    EXPECT_EQ(info.find(&points, "home"_atom), &points.at("home"));
}

TEST(TypeInfo, MapsWithOtherKeysAreValues)
{
    EXPECT_EQ((TypeInfoOf<std::map<int, int>>().kind()), TypeKind::Value);
}

// ===================== Descriptors =====================

TEST(TypeInfo, EveryPropertyCarriesTheInfoOfItsType)
{
    const auto& properties = Describe<Containers>().properties;
    std::vector<TypeKind> kinds;
    for (const auto& property : properties)
    {
        ASSERT_NE(property.typeInfo, nullptr) << property.name.toStdString();
        EXPECT_EQ(property.typeInfo->type(), property.type);
        kinds.push_back(property.typeInfo->kind());
    }

    EXPECT_THAT(kinds, ElementsAre(TypeKind::Enum, TypeKind::Optional, TypeKind::Sequence,
                                   TypeKind::Sequence, TypeKind::Map, TypeKind::Map,
                                   TypeKind::Class, TypeKind::Value, TypeKind::Value));
}
