// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Stringifier.h"

namespace NX::Stringify
{
    const std::string_view gcDefaultNoneString = "None";

    std::string GetNoneIfEmpty(std::string_view str)
    {
        if (str.empty())
        {
            return { gcDefaultNoneString.data(), gcDefaultNoneString.size() };
        }

        return { str.data(), str.size() };
    }

} // namespace NX::Stringify
