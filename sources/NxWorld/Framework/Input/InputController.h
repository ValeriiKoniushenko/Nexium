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
#include "NxWorld/Framework/Input/InputActionTypes.h"
#include "glm/vec2.hpp"

#include <concepts>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace NX
{
    class InputSystem;

    /// Well-known InputController priorities. A higher number is offered an event first.
    namespace InputPriority
    {
        /// Gameplay controllers, such as spectators and the player.
        constexpr int Gameplay = 0;
        /// Editor controllers that should win while the editor, not the game, has the user's
        /// attention.
        constexpr int Editor = 100;
        /// A controller owned by a focused editor window, such as an asset editor. It wins over
        /// the editor-wide shortcuts.
        constexpr int FocusedEditorWindow = 200;
        /// Editor controllers that only get what the game does not bind, while the game
        /// viewport has the user's attention.
        constexpr int EditorBehindGameplay = -100;
    } // namespace InputPriority

    CLASS();
    class InputController : public NX::BaseComponent
    {
        ECS_DECL(InputController, NX::BaseComponent);

    public:
        ~InputController() override;

        using ActionCallback = std::function<void()>;

        bool addAction(InputAction action);
        bool addAction(const Core::StringAtom& name, std::vector<InputBinding> bindings);
        bool addAction(const Core::StringAtom& name, std::vector<InputBinding> bindings,
                       ActionCallback onTriggered);

        void onTriggered(const Core::StringAtom& action, ActionCallback callback);
        void clearCallbacks(const Core::StringAtom& action);
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

        /// Controllers with a higher priority are offered an event first. Equal priorities all
        /// receive it. Takes effect on the next processEvents().
        [[nodiscard]] int getPriority() const noexcept { return _priority; }
        void setPriority(int priority) noexcept { _priority = priority; }

        /// When true (the default), an enabled controller that has a binding for an event keeps
        /// every lower-priority controller from seeing it. When false it only observes.
        [[nodiscard]] bool doesConsumeInput() const noexcept { return _consumesInput; }
        void setConsumesInput(bool consumes) noexcept { _consumesInput = consumes; }

        [[nodiscard]] spdlog::logger* getLogger() const override;

    protected:
        void onInitialize() override;
        void onDeinitialize() override;

    private:
        friend class NX::InputSystem;

        void beginInputFrame();
        /// The three below return whether this controller has a binding that accepts the event.
        [[nodiscard]] bool onButton(InputSource source, std::uint16_t code, InputState state,
                                    InputModifier mods);
        [[nodiscard]] bool onVector(InputSource source, glm::vec2 value);
        [[nodiscard]] bool onAxis(std::uint16_t axis, float value);
        void endInputFrame();
        /// Releases held buttons and axes when the window stops receiving input.
        void releaseHeldInput();
        void dispatchTriggeredCallbacks();

        /// Like findAction(), but logs a warning (once per name) when the action is missing.
        [[nodiscard]] const InputAction* findActionOrWarn(const Core::StringAtom& name) const;
        [[nodiscard]] glm::vec2 getVectorValue(const Core::StringAtom& action) const;

        FIELD();
        std::vector<NX::InputAction> _actions;

        FIELD();
        // The generated code cannot see InputPriority, so this is InputPriority::Gameplay.
        int _priority = 0;

        FIELD();
        bool _consumesInput = true;

        mutable std::unordered_set<Core::StringAtom> _reportedMissingActions;
        std::unordered_map<Core::StringAtom, std::vector<ActionCallback>> _triggeredCallbacks;
    };
} // namespace NX

#include "InputController.generated.h" // added by the code generator. Better don't move it.
