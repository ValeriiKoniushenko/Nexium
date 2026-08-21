#pragma once

#include "Core/String.h"

#include <cstddef>

namespace Core::Animation
{
    struct AnimationEvent
    {
        StringAtom animationName;
        std::size_t frameIndex = 0;
        StringAtom eventName;
    };
} // namespace Core::Animation
