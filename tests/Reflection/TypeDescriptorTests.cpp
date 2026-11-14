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

#include <string>
#include <typeindex>
#include <utility>
#include <vector>

using namespace Core;
using namespace ReflectionFixtures;
using NX::Reflection::ClassDesc;
using NX::Reflection::Describe;
using NX::Reflection::ObjectRef;
using NX::Reflection::PropertyDesc;
using testing::ElementsAre;

ECS_IMPL(ReflectionFixtures::SpyComponent);

namespace
{
    std::vector<std::string> PropertyNames(const ClassDesc& desc)
    {
        std::vector<std::string> out;
        for (const PropertyDesc& property : desc.properties)
        {
            out.push_back(property.name.toStdString());
        }
        return out;
    }

    const PropertyDesc& PropertyOf(const ClassDesc& desc, const char* name)
    {
        const auto* property = desc.findOwnProperty(StringAtom::Intern(name));
        EXPECT_NE(property, nullptr) << name;
        return *property;
    }
} // namespace

// ===================== Descriptors of plain types =====================

TEST(TypeDescriptor, ListsOwnPropertiesInDeclarationOrder)
{
    const ClassDesc& desc = Describe<Plain>();

    EXPECT_THAT(PropertyNames(desc), ElementsAre("speed", "title", "id", "owned"));
    EXPECT_EQ(desc.properties[0].type, std::type_index(typeid(float)));
    EXPECT_EQ(desc.properties[1].type, std::type_index(typeid(std::string)));
    EXPECT_EQ(desc.properties[2].type, std::type_index(typeid(int)));
    EXPECT_EQ(desc.properties[3].type,
              std::type_index(typeid(std::vector<std::unique_ptr<int>>)));
}

TEST(TypeDescriptor, IsBuiltOnceAndNamesTheType)
{
    const ClassDesc& desc = Describe<Plain>();

    EXPECT_EQ(&desc, &Describe<Plain>());
    EXPECT_EQ(desc.name, "ReflectionFixtures::Plain");
    EXPECT_EQ(desc.shortName, "Plain");
    EXPECT_EQ(desc.type, std::type_index(typeid(Plain)));
    EXPECT_TRUE(desc.bases.empty());
}

TEST(TypeDescriptor, PointsAtTheGeneratedFieldMetadata)
{
    const ClassDesc& desc = Describe<Plain>();
    const auto& fields = R<Plain>::GetFieldsArray();

    for (std::size_t i = 0; i < fields.size(); ++i)
    {
        EXPECT_EQ(desc.properties[i].field, &fields[i]);
    }
    EXPECT_EQ(desc.properties[0].field->attributes.getMeta("DisplayName"),
              std::optional<std::string_view>("Speed"));
}

TEST(TypeDescriptor, GetGivesTypedAccessOnlyForTheExactType)
{
    Plain plain;
    const ClassDesc& desc = Describe<Plain>();
    const PropertyDesc& speed = PropertyOf(desc, "speed");

    EXPECT_EQ(speed.get<float>(&plain), &plain.speed);
    EXPECT_EQ(speed.get<int>(&plain), nullptr);
    EXPECT_EQ(speed.get<double>(&plain), nullptr);
}

TEST(TypeDescriptor, ConstFieldsAreReadableButNotMutable)
{
    Plain plain;
    const PropertyDesc& id = PropertyOf(Describe<Plain>(), "id");

    EXPECT_TRUE(id.isConst);
    EXPECT_EQ(id.get<int>(&plain), nullptr);

    const Plain& constPlain = plain;
    const int* value = id.get<int>(static_cast<const void*>(&constPlain));
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, 5);
}

TEST(TypeDescriptor, SetWritesTheFieldWhenNoSetterIsDeclared)
{
    Plain plain;
    const PropertyDesc& title = PropertyOf(Describe<Plain>(), "title");

    EXPECT_FALSE(title.hasSetter);
    EXPECT_TRUE(title.isEditable());
    EXPECT_TRUE(title.set(&plain, std::string("Hello")));
    EXPECT_EQ(plain.title, "Hello");
}

TEST(TypeDescriptor, SetRejectsAValueOfAnotherType)
{
    Plain plain;
    const PropertyDesc& speed = PropertyOf(Describe<Plain>(), "speed");

    EXPECT_FALSE(speed.set(&plain, 3));
    EXPECT_EQ(plain.speed, 1.f);
}

TEST(TypeDescriptor, ConstAndNonCopyableFieldsWithoutSetterAreNotEditable)
{
    Plain plain;
    const ClassDesc& desc = Describe<Plain>();

    EXPECT_FALSE(PropertyOf(desc, "id").isEditable());
    EXPECT_FALSE(PropertyOf(desc, "id").set(&plain, 9));
    EXPECT_FALSE(PropertyOf(desc, "owned").isEditable());
}

TEST(TypeDescriptor, SetGoesThroughTheDeclaredSetterAndNotifiesTheOwner)
{
    Edited edited;
    const ClassDesc& desc = Describe<Edited>();
    const PropertyDesc& level = PropertyOf(desc, "level");
    const PropertyDesc& plainValue = PropertyOf(desc, "plainValue");

    EXPECT_TRUE(level.hasSetter);
    EXPECT_TRUE(level.set(&edited, 4));
    EXPECT_EQ(edited.level, 40);

    EXPECT_FALSE(plainValue.hasSetter);
    EXPECT_TRUE(plainValue.set(&edited, 2.5f));
    EXPECT_EQ(edited.plainValue, 2.5f);

    EXPECT_THAT(edited.edited, ElementsAre("level"_atom, "plainValue"_atom));
}

TEST(TypeDescriptor, RawAddressWritesDoNotNotify)
{
    Edited edited;
    const PropertyDesc& plainValue = PropertyOf(Describe<Edited>(), "plainValue");

    *plainValue.get<float>(&edited) = 7.f;

    EXPECT_EQ(edited.plainValue, 7.f);
    EXPECT_TRUE(edited.edited.empty());
}

// ===================== Inheritance =====================

TEST(TypeDescriptor, ChainVisitsBasesInRFriendOrderThenTheClassWithAdjustedPointers)
{
    Derived derived;
    std::vector<std::pair<std::string, void*>> visited;

    Describe<Derived>().forEachInChain(&derived,
                                       [&](const ClassDesc& desc, void* owner)
                                       { visited.emplace_back(desc.shortName.toStdString(), owner); });

    using Visit = std::pair<std::string, void*>;
    ASSERT_THAT(visited, testing::SizeIs(3));
    EXPECT_EQ(visited[0], Visit("Left", static_cast<Left*>(&derived)));
    EXPECT_EQ(visited[1], Visit("Right", static_cast<Right*>(&derived)));
    EXPECT_EQ(visited[2], Visit("Derived", &derived));
    EXPECT_NE(static_cast<void*>(static_cast<Right*>(&derived)), static_cast<void*>(&derived));
}

TEST(TypeDescriptor, ObjectRefFindsTheMostDerivedPropertyFirst)
{
    Derived derived;
    const ObjectRef ref(derived);

    const auto shared = ref.findProperty("shared"_atom);
    ASSERT_TRUE(shared);
    EXPECT_EQ(*shared.get<int>(), 30);

    const auto right = ref.findProperty("right"_atom);
    ASSERT_TRUE(right);
    EXPECT_EQ(right.owner, static_cast<Right*>(&derived));
    EXPECT_TRUE(right.set(5));
    EXPECT_EQ(derived.right, 5);

    EXPECT_FALSE(ref.findProperty("missing"_atom));
}

TEST(TypeDescriptor, ObjectRefVisitsEveryPropertyOfTheChain)
{
    Derived derived;
    std::vector<std::string> names;

    ObjectRef(derived).forEachProperty([&](const ClassDesc& desc, const auto& property)
                                       { names.push_back(desc.shortName.toStdString() + "."
                                                         + property.property->name.toStdString()); });

    EXPECT_THAT(names, ElementsAre("Left.left", "Right.right", "Right.shared", "Derived.derived",
                                   "Derived.shared"));
}

// ===================== Components =====================

TEST(TypeDescriptor, ComponentReflectsItsMostDerivedTypeThroughABasePointer)
{
    auto component = SpyComponent::Create();
    NX::BaseComponent& base = *component;

    const ObjectRef ref = base.reflect();

    ASSERT_NE(ref.desc, nullptr);
    EXPECT_EQ(ref.desc, &Describe<SpyComponent>());
    EXPECT_EQ(ref.object, static_cast<void*>(component.get()));

    const NX::BaseComponent& constBase = base;
    EXPECT_EQ(constBase.reflect().desc, ref.desc);
    EXPECT_EQ(&constBase.getClassDesc(), ref.desc);
}

TEST(TypeDescriptor, ComponentChainStartsAtTheEngineRoot)
{
    auto component = SpyComponent::Create();
    std::vector<std::string> classes;

    component->getClassDesc().forEachInChain(component.get(),
                                             [&](const ClassDesc& desc, void*)
                                             { classes.push_back(desc.shortName.toStdString()); });

    EXPECT_THAT(classes, ElementsAre("AbstractComponent", "BaseComponent", "SpyComponent"));
    EXPECT_THAT(PropertyNames(Describe<NX::AbstractComponent>()),
                ElementsAre("_isEnabled", "_noTick"));
}

TEST(TypeDescriptor, EditingAComponentThroughItsBaseNotifiesTheMostDerivedOverride)
{
    auto component = SpyComponent::Create();
    NX::BaseComponent& base = *component;
    const ObjectRef ref = base.reflect();

    EXPECT_TRUE(ref.findProperty("power"_atom).set(5));
    EXPECT_TRUE(ref.findProperty("_isEnabled"_atom).set(false));

    EXPECT_EQ(component->power, 5);
    EXPECT_FALSE(component->isEnabled());
    EXPECT_THAT(component->edited, ElementsAre("power"_atom, "_isEnabled"_atom));
}
