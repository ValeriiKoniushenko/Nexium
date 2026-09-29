// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "InputTypes.h"

#include <algorithm>
#include <ranges>

namespace NX
{
    KeyChord KeyChord::Exact(Platform::Keyboard::Key key)
    {
        return { .triggerKey = key };
    }

    bool KeyChord::matches(Platform::Keyboard::Key eventKey,
                           const std::vector<Platform::Keyboard::Key>& pressedKeys) const
    {
        if (triggerKey != eventKey)
        {
            return false;
        }

        return std::ranges::all_of(
            requiredKeys, [&pressedKeys](Platform::Keyboard::Key key)
            { return std::ranges::find(pressedKeys, key) != pressedKeys.end(); });
    }

    bool KeyChord::contains(Platform::Keyboard::Key key) const
    {
        return triggerKey == key || std::ranges::find(requiredKeys, key) != requiredKeys.end();
    }
} // namespace NX
