#pragma once

#include "Core/String.h"

#include <cstddef>

namespace Core::Animation
{
    /// @brief Event emitted when a frame with a named marker becomes active.
    ///
    /// The animator emits events in frame order, including every crossed frame when a large delta
    /// advances more than one frame. Consumers such as AnimationAudioBinding use the names as keys.
    struct AnimationEvent
    {
        StringAtom animationName;
        std::size_t frameIndex = 0;
        StringAtom eventName;
    };
} // namespace Core::Animation
