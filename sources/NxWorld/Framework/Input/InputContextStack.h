// Nexium
// Copyright 2018-2026 Valerii Koniushenko
// Licensed under the Apache License, Version 2.0.

#pragma once

#include "NxSubsystems/Input/InputTypes.h"

#include <optional>

namespace NX
{
    // tag::input-context-layer[]
    enum class InputContextPolicy
    {
        ConsumeMatched,
        BlockLower
    };

    struct InputContextLayer
    {
        InputContext context = InputContext::Editor;
        InputContextPolicy policy = InputContextPolicy::ConsumeMatched;
        InputChannel channels = InputChannel::All;
    };
    // end::input-context-layer[]

    class InputContextStack
    {
    public:
        // tag::input-context-stack-api[]
        using LayerId = std::uint64_t;

        bool setActiveContext(InputContext context);
        bool setGlobalContext(std::optional<InputContext> context);
        [[nodiscard]] LayerId push(InputContextLayer layer);
        bool pop(LayerId id);
        [[nodiscard]] std::vector<InputContextLayer> getLayers() const;
        [[nodiscard]] bool isAvailable(InputContext context, InputChannel channel) const;
        // end::input-context-stack-api[]

    private:
        struct Entry
        {
            LayerId id;
            InputContextLayer layer;
        };

        InputContext _activeContext = InputContext::Editor;
        std::optional<InputContext> _globalContext;
        std::vector<Entry> _overlays;
        LayerId _nextId = 1;
    };
} // namespace NX
