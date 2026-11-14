// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Color.h"
#include "NxFundamental/ECS/BaseComponent.h"
#include "NxFundamental/Reflection/TypeDescriptor.h"

#include <optional>
#include <unordered_map>
#include <vector>

namespace GridFixtures
{
    ENUM_CLASS();
    enum class Quality
    {
        Low,
        Medium,
        High
    };

    CLASS();
    struct Inner
    {
        R_FRIEND(Inner);

        FIELD();
        int a = 1;

        FIELD();
        float b = 2.f;
    };

    CLASS();
    class Settings
    {
        R_FRIEND(Settings);

    public:
        void setLevel(int value) { level = value * 10; }

        void onFieldEdited(const NX::Reflection::PropertyDesc& property)
        {
            edited.push_back(property.name);
        }

        std::vector<Core::StringAtom> edited;

        FIELD(R::Min = 0.0, R::Max = 10.0, R::Step = 0.5);
        float strength = 1.f;

        FIELD(R::Meta(DisplayName = "Light color"));
        Core::NormColor3 color = Core::NormColor3(1.f);

        FIELD();
        bool enabled = true;

        FIELD();
        GridFixtures::Quality quality = GridFixtures::Quality::Medium;

        FIELD();
        std::optional<float> limit;

        FIELD();
        std::vector<int> values = { 1, 2 };

        FIELD();
        GridFixtures::Inner inner;

        FIELD(R::Meta(Hidden));
        int secret = 0;

        FIELD(R::Meta(ReadOnly));
        int locked = 3;

        FIELD(R::Meta(Setter = setLevel));
        int level = 0;

        FIELD(R::Min = 2);
        std::size_t count = 5;

        FIELD();
        glm::vec3 direction = glm::vec3(0.f);

        FIELD();
        std::unordered_map<Core::StringAtom, float> weights;
    };

    CLASS();
    struct OtherSettings
    {
        R_FRIEND(OtherSettings);

        FIELD();
        int value = 9;
    };

    CLASS();
    class GridComponent : public NX::BaseComponent
    {
        ECS_DECL(GridComponent, NX::BaseComponent);

    public:
        void onFieldEdited(const NX::Reflection::PropertyDesc& property) override
        {
            edited.push_back(property.name);
        }

        std::vector<Core::StringAtom> edited;

        FIELD();
        float speed = 2.f;
    };
} // namespace GridFixtures

#include "PropertyGridFixtures.generated.h" // added by the code generator. Better don't move it.
