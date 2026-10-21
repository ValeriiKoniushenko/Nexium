// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "NxSubsystems/Input/InputTypes.h"
#include "nlohmann/json_fwd.hpp"

#include <optional>
#include <span>
#include <utility>
#include <vector>

namespace NX
{
    // tag::input-binding[]
    struct InputBinding
    {
        Core::StringAtom action{};
        KeyChord chord{};
        InputActionTrigger trigger = InputActionTrigger::OnPress;
        InputEventType source = InputEventType::Button;
    };
    // end::input-binding[]

    /// Editable configuration only. Each controller has its own action runtime.
    // tag::input-map-api[]
    class InputMap
    {
    public:
        InputMap() = default;
        explicit InputMap(std::vector<InputBinding> bindings);

        [[nodiscard]] const std::vector<InputBinding>& getBindings() const noexcept;

        void setBindings(std::vector<InputBinding> bindings);

        bool bind(InputBinding binding);
        bool unbind(const Core::StringAtom& action);

        [[nodiscard]] std::optional<InputBinding> resolveBinding(const KeyInputEvent& event) const;

        [[nodiscard]] static bool IsValid(const InputBinding& binding);
        static bool Bind(std::vector<InputBinding>& bindings, InputBinding binding);
        static bool Unbind(std::vector<InputBinding>& bindings, const Core::StringAtom& action);
        [[nodiscard]] static std::optional<InputBinding> ResolveBinding(
            std::span<const InputBinding> bindings, const KeyInputEvent& event);
        // end::input-map-api[]

    private:
        std::vector<InputBinding> _bindings;
    };

    void to_json(nlohmann::json& json, const InputBinding& binding);
    void from_json(const nlohmann::json& json, InputBinding& binding);
} // namespace NX
