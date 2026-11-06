// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::World

#pragma once

#include "NxFundamental/ECS/BaseComponent.h"
#include "NxWorld/Framework/Input/InputActionState.h"
#include "NxWorld/Framework/Input/InputActionTypes.h"
#include "glm/vec2.hpp"

#include <concepts>
#include <cstdint>
#include <unordered_set>
#include <vector>

namespace NX
{
    class InputSystem;

    CLASS();
    class InputController : public NX::BaseComponent
    {
        ECS_DECL(InputController, NX::BaseComponent);

    public:
        ~InputController() override;

        bool addAction(InputAction action);
        bool removeAction(const Core::StringAtom& name);

        [[nodiscard]] const InputAction* findAction(const Core::StringAtom& name) const;
        [[nodiscard]] InputAction* findAction(const Core::StringAtom& name);

        [[nodiscard]] const std::vector<InputAction>& getActions() const noexcept
        {
            return _actions;
        }

        [[nodiscard]] bool isActive(const Core::StringAtom& action) const;
        [[nodiscard]] bool wasTriggered(const Core::StringAtom& action) const;

        template<typename T>
            requires std::same_as<T, bool> || std::same_as<T, float> || std::same_as<T, glm::vec2>
        [[nodiscard]] T getValue(const Core::StringAtom& action) const
        {
            const glm::vec2 value = getVectorValue(action);
            if constexpr (std::same_as<T, bool>)
            {
                return value.x != 0.f || value.y != 0.f;
            }
            else if constexpr (std::same_as<T, float>)
            {
                return value.x;
            }
            else
            {
                return value;
            }
        }

        [[nodiscard]] spdlog::logger* getLogger() const override;

    protected:
        void onInitialize() override;
        void onDeinitialize() override;

    private:
        friend class NX::InputSystem;

        void beginInputFrame();
        void onButton(InputSource source, std::uint16_t code, InputState state, InputModifier mods);
        void onVector(InputSource source, glm::vec2 value);
        void onAxis(std::uint16_t axis, float value);
        void endInputFrame();

        template<typename Function>
        void forEachBinding(const Function& function);

        [[nodiscard]] bool statesMatchActions() const;
        void ensureStatesMatchActions();
        [[nodiscard]] const ActionState* findState(const Core::StringAtom& action) const;
        [[nodiscard]] glm::vec2 getVectorValue(const Core::StringAtom& action) const;

        FIELD();
        std::vector<NX::InputAction> _actions;

        std::vector<ActionState> _actionStates;
        mutable std::unordered_set<Core::StringAtom> _reportedMissingActions;
    };
} // namespace NX

#include "InputController.generated.h" // added by the code generator. Better don't move it.
