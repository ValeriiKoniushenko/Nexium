// Nexium
// Copyright 2018-2026 Valerii Koniushenko
// Licensed under the Apache License, Version 2.0.

#include "InputContextStack.h"

#include <algorithm>

namespace NX
{
    bool InputContextStack::setActiveContext(InputContext context)
    {
        if (_activeContext == context)
        {
            return false;
        }
        _activeContext = context;
        return true;
    }

    bool InputContextStack::setGlobalContext(std::optional<InputContext> context)
    {
        if (_globalContext == context)
        {
            return false;
        }
        _globalContext = context;
        return true;
    }

    InputContextStack::LayerId InputContextStack::push(InputContextLayer layer)
    {
        const auto id = _nextId++;
        _overlays.push_back({ .id = id, .layer = layer });
        return id;
    }

    bool InputContextStack::pop(LayerId id)
    {
        return std::erase_if(_overlays, [id](const Entry& entry) { return entry.id == id; }) != 0;
    }

    std::vector<InputContextLayer> InputContextStack::getLayers() const
    {
        std::vector<InputContextLayer> layers;

        for (auto entry : _overlays)
        {
            layers.push_back(entry.layer);
        }

        if (_globalContext && *_globalContext != _activeContext)
        {
            layers.push_back({ .context = *_globalContext });
        }
        layers.push_back({ .context = _activeContext });
        return layers;
    }

    bool InputContextStack::isAvailable(InputContext context, InputChannel channel) const
    {
        for (const auto& layer : getLayers())
        {
            if (!hasInputChannel(layer.channels, channel))
            {
                continue;
            }
            if (layer.context == context)
            {
                return true;
            }
            if (layer.policy == InputContextPolicy::BlockLower)
            {
                return false;
            }
        }
        return false;
    }
} // namespace NX
