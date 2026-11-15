// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "NxFundamental/ECS/BaseComponent.h"
#include "NxFundamental/Reflection/TypeDescriptor.h"

#include <array>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace ReflectionFixtures
{
    CLASS();
    struct Plain
    {
        R_FRIEND(Plain);

        FIELD(R::Min = 0.0, R::Meta(DisplayName = "Speed"));
        float speed = 1.f;

        FIELD();
        std::string title = "Untitled";

        FIELD();
        const int id = 5;

        FIELD(R::NoDefaultValue);
        std::vector<std::unique_ptr<int>> owned;
    };

    CLASS();
    class Edited
    {
        R_FRIEND(Edited);

    public:
        void setLevel(int value) { level = value * 10; }

        void onFieldEdited(const NX::Reflection::PropertyDesc& property)
        {
            edited.push_back(property.name);
        }

        std::vector<Core::StringAtom> edited;

        FIELD(R::Meta(Setter = setLevel));
        int level = 0;

        FIELD();
        float plainValue = 0.f;
    };

    CLASS();
    struct Left
    {
        R_FRIEND(Left);

        FIELD();
        int left = 1;
    };

    CLASS();
    struct Right
    {
        R_FRIEND(Right);

        FIELD();
        int right = 2;

        FIELD();
        int shared = 20;
    };

    struct Unreflected
    {
        int padding = 0;
    };

    // `Unreflected` sits between the reflected bases, so `Right` is not at offset 0.
    CLASS();
    struct Derived : Left, Unreflected, Right
    {
        R_FRIEND(Derived, ReflectionFixtures::Left, ReflectionFixtures::Right);

        FIELD();
        int derived = 3;

        FIELD();
        int shared = 30;
    };

    CLASS();
    class SpyComponent : public NX::BaseComponent
    {
        ECS_DECL(SpyComponent, NX::BaseComponent);

    public:
        void onFieldEdited(const NX::Reflection::PropertyDesc& property) override
        {
            edited.push_back(property.name);
        }

        std::vector<Core::StringAtom> edited;

        FIELD();
        int power = 1;
    };

    ENUM_CLASS();
    enum class Mode
    {
        Off,
        Low,
        High = 4
    };

    CLASS();
    struct Point
    {
        R_FRIEND(Point);

        FIELD();
        int x = 1;

        FIELD();
        int y = 2;
    };

    CLASS();
    struct Containers
    {
        R_FRIEND(Containers);

        FIELD();
        ReflectionFixtures::Mode mode = ReflectionFixtures::Mode::Low;

        FIELD();
        std::optional<float> limit;

        FIELD();
        std::vector<int> values = { 1, 2, 3 };

        FIELD();
        std::array<int, 2> pair = { 7, 8 };

        FIELD();
        std::unordered_map<Core::StringAtom, int> named;

        FIELD();
        std::map<std::string, ReflectionFixtures::Point> points;

        FIELD();
        ReflectionFixtures::Point origin;

        FIELD();
        std::string text;

        FIELD();
        std::map<int, int> byNumber;
    };
} // namespace ReflectionFixtures

#include "ReflectionFixtures.generated.h" // added by the code generator. Better don't move it.
