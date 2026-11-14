// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Fundamental::Reflection

#include "TypeDescriptor.h"

namespace NX::Reflection
{
    const PropertyDesc* ClassDesc::findOwnProperty(const Core::StringAtom& name) const
    {
        const auto it = std::ranges::find(properties, name, &PropertyDesc::name);
        return it != properties.end() ? &*it : nullptr;
    }

    PropertyRef ObjectRef::findProperty(const Core::StringAtom& name) const
    {
        // Chain order is root-first, so the last match is the most-derived declaration.
        PropertyRef found;
        if (*this)
        {
            desc->forEachInChain(object,
                                 [&](const ClassDesc& cls, void* owner)
                                 {
                                     if (const auto* property = cls.findOwnProperty(name))
                                     {
                                         found = PropertyRef{ property, owner };
                                     }
                                 });
        }
        return found;
    }
} // namespace NX::Reflection
