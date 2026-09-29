/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 */

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
