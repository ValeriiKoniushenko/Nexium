// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Fundamental::ResourceManagement

#pragma once

#include "JustReflectMe/Adapter.h"

#define R_FRIEND_DECL(Class, ...)                                                                  \
    R_FRIEND(Class, __VA_ARGS__);                                                                  \
                                                                                                   \
public:                                                                                            \
    [[nodiscard]] nlohmann::json serialize() const override;                                       \
    void deserialize(RResourceStream<RJsonResourceStream>& stream) override

#define R_FRIEND_IMPL(TypeName)                                                                    \
    nlohmann::json TypeName::serialize() const                                                     \
    {                                                                                              \
        return R<TypeName>::Serialize<RJsonResourceStream>(*this).getData();                       \
    }                                                                                              \
    void TypeName::deserialize(RResourceStream<RJsonResourceStream>& stream)                       \
    {                                                                                              \
        R<TypeName>::Deserialize(stream, *this);                                                   \
    }
