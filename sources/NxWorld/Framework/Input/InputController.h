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
#include "NxFundamental/ResourceManagement/JsonAdapter.h"
#include "Platform/Input/Events/InputEvent.h"
#include "glm/vec2.hpp"

#include <concepts>
#include <cstdint>
#include <unordered_set>
#include <vector>

namespace NX
{
    class InputSystem;

    ENUM_CLASS();
    enum class InputSource : std::uint8_t
    {
        Key,
        MouseButton,
        MouseDelta,
        MouseScroll,
        GamepadButton,
        GamepadAxis
    };

    ENUM_CLASS();
    enum class InputTrigger : std::uint8_t
    {
        Down,
        Pressed,
        Released
    };

    CLASS();
    struct InputBinding
    {
        R_FRIEND(InputBinding);

        FIELD();
        NX::InputSource source = NX::InputSource::Key;
        FIELD();
        std::uint16_t code = 0;
        FIELD();
        NX::InputTrigger trigger = NX::InputTrigger::Down;
        FIELD();
        bool shift = false;
        FIELD();
        bool control = false;
        FIELD();
        bool alt = false;
        FIELD();
        bool super = false;
        FIELD();
        bool negate = false;
        FIELD();
        bool swizzleYX = false;

        [[nodiscard]] InputModifier getModifiers() const noexcept;
    };

    void to_json(nlohmann::json& j, const InputBinding& v);
    void from_json(const nlohmann::json& j, InputBinding& v);

    CLASS();
    struct InputAction
    {
        R_FRIEND(InputAction);

        FIELD();
        Core::StringAtom name;
        FIELD();
        std::vector<NX::InputBinding> bindings;
    };

    void to_json(nlohmann::json& j, const InputAction& v);
    void from_json(const nlohmann::json& j, InputAction& v);

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

        struct BindingState
        {
            bool isHeld = false;
            bool wasPressedThisFrame = false;
            bool firedThisFrame = false;
            glm::vec2 value{ 0.f };
        };

        struct ActionState
        {
            std::vector<BindingState> bindings;
            glm::vec2 value{ 0.f };
            bool isActive = false;
            bool wasTriggered = false;
        };

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
