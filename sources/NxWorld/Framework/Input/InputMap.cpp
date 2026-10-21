// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "InputMap.h"

#include <algorithm>
#include <ranges>

namespace
{
    Core::StringAtom MouseButtonName(Platform::Mouse::Key button)
    {
        using Button = Platform::Mouse::Key;
        switch (button)
        {
            case Button::Left:
                return "Left"_atom;
            case Button::Right:
                return "Right"_atom;
            case Button::Middle:
                return "Middle"_atom;
            case Button::_4:
                return "_4"_atom;
            case Button::_5:
                return "_5"_atom;
            case Button::_6:
                return "_6"_atom;
            case Button::_7:
                return "_7"_atom;
            case Button::_8:
                return "_8"_atom;
            default:
                return "None"_atom;
        }
    }

    nlohmann::json SerializeInputButton(NX::InputButton button)
    {
        if (button.isMouse())
        {
            return { { "device", "Mouse" },
                     { "button", MouseButtonName(button.getMouseButton()).toStdString() } };
        }
        return std::string{ R<Platform::Keyboard::Key>::ToString(button.getKeyboardKey()) };
    }

    std::optional<NX::InputButton> DeserializeInputButton(const nlohmann::json& value)
    {
        if (value.is_string())
        {
            if (const auto key = R<Platform::Keyboard::Key>::FromString(value.get<std::string>()))
            {
                return *key;
            }
        }
        else if (value.is_number_integer())
        {
            return static_cast<Platform::Keyboard::Key>(value.get<int>());
        }
        else if (value.is_object() && value.contains("device") && value.at("device").is_string()
                 && value.at("device").get<std::string>() == "Mouse" && value.contains("button"))
        {
            const auto& button = value.at("button");
            if (button.is_string())
            {
                if (const auto key = R<Platform::Mouse::Key>::FromString(button.get<std::string>()))
                {
                    return *key;
                }
            }
        }
        return std::nullopt;
    }
} // namespace

namespace NX
{
    void to_json(nlohmann::json& json, const InputBinding& binding)
    {
        json = nlohmann::json{ { "action", binding.action },
                               { "triggerKey", SerializeInputButton(binding.chord.triggerKey) },
                               { "trigger", binding.trigger == InputActionTrigger::WhileHeld
                                                ? "WhileHeld"
                                                : (binding.trigger == InputActionTrigger::OnRelease
                                                       ? "OnRelease"
                                                       : "OnPress") } };
        if (binding.source != InputEventType::Button)
        {
            json["source"]
                = binding.source == InputEventType::PointerMove ? "PointerMove" : "Wheel";
        }
        auto& keys = json["requiredKeys"] = nlohmann::json::array();
        for (const auto key : binding.chord.requiredKeys)
        {
            keys.push_back(SerializeInputButton(key));
        }
    }

    void from_json(const nlohmann::json& json, InputBinding& binding)
    {
        binding = {};
        if (json.contains("action"))
        {
            binding.action = Core::StringAtom::Intern(json.at("action").get<Core::StringAtom>());
        }

        const auto* const keyField = json.contains("triggerKey") ? "triggerKey" : "key";
        if (json.contains(keyField))
        {
            if (const auto button = DeserializeInputButton(json.at(keyField)))
            {
                binding.chord.triggerKey = *button;
            }
        }
        for (const auto& value : json.value("requiredKeys", nlohmann::json::array()))
        {
            if (const auto button = DeserializeInputButton(value))
            {
                binding.chord.requiredKeys.push_back(*button);
            }
        }

        const auto source = json.value("source", std::string{ "Button" });
        if (source == "PointerMove")
        {
            binding.source = InputEventType::PointerMove;
        }
        else if (source == "Wheel")
        {
            binding.source = InputEventType::Wheel;
        }

        // Compatibility with assets saved before modifiers became regular chord keys.
        const auto oldModifiers = json.value("requiredModifiers", std::uint8_t{});
        const auto appendOldModifier
            = [&binding, oldModifiers](std::uint8_t modifier, Platform::Keyboard::Key key)
        {
            if ((oldModifiers & static_cast<std::uint8_t>(modifier)) != 0
                && std::ranges::find(binding.chord.requiredKeys, key)
                       == binding.chord.requiredKeys.end())
            {
                binding.chord.requiredKeys.push_back(key);
            }
        };
        appendOldModifier(InputModifier_Control, Platform::Keyboard::Key::Left_Control);
        appendOldModifier(InputModifier_Shift, Platform::Keyboard::Key::Left_Shift);
        appendOldModifier(InputModifier_Alt, Platform::Keyboard::Key::Left_Alt);
        appendOldModifier(InputModifier_Super, Platform::Keyboard::Key::Left_Super);
        const auto trigger = json.value("trigger", std::string{ "OnPress" });
        if (trigger == "WhileHeld")
        {
            binding.trigger = InputActionTrigger::WhileHeld;
        }
        else if (trigger == "OnRelease")
        {
            binding.trigger = InputActionTrigger::OnRelease;
        }
        else
        {
            binding.trigger = InputActionTrigger::OnPress;
        }
    }

    bool InputMap::IsValid(const InputBinding& binding)
    {
        return !binding.action.isEmpty()
               && (binding.source != InputEventType::Button || !binding.chord.triggerKey.isNone());
    }

    bool InputMap::Bind(std::vector<InputBinding>& bindings, InputBinding binding)
    {
        if (!IsValid(binding))
        {
            return false;
        }
        const auto existing = std::ranges::find_if(bindings, [&binding](const InputBinding& value)
                                                   { return value.action == binding.action; });
        if (existing != bindings.end())
        {
            *existing = std::move(binding);
            return false;
        }
        bindings.push_back(std::move(binding));
        return true;
    }

    bool InputMap::Unbind(std::vector<InputBinding>& bindings, const Core::StringAtom& action)
    {
        return std::erase_if(bindings, [&action](const InputBinding& binding)
                             { return binding.action == action; })
               != 0;
    }

    std::optional<InputBinding> InputMap::ResolveBinding(std::span<const InputBinding> bindings,
                                                         const KeyInputEvent& event)
    {
        const InputBinding* best = nullptr;
        for (const auto& candidate : bindings)
        {
            if (!IsValid(candidate) || candidate.source != event.type)
            {
                continue;
            }
            const auto eventKey
                = event.type == InputEventType::Button ? event.key : candidate.chord.triggerKey;
            if (!candidate.chord.matches(eventKey, event.pressedKeys))
            {
                continue;
            }
            if (event.type != InputEventType::Button && !candidate.chord.triggerKey.isNone()
                && std::ranges::find(event.pressedKeys,
                                     NormalizeInputButton(candidate.chord.triggerKey))
                       == event.pressedKeys.end())
            {
                continue;
            }
            const auto specificity = candidate.chord.requiredKeys.size()
                                     + (candidate.chord.triggerKey.isNone() ? 0 : 1);
            const auto bestSpecificity
                = best ? best->chord.requiredKeys.size() + (best->chord.triggerKey.isNone() ? 0 : 1)
                       : 0;
            if (!best || specificity > bestSpecificity)
            {
                best = &candidate;
            }
        }
        return best ? std::optional<InputBinding>{ *best } : std::nullopt;
    }

    InputMap::InputMap(std::vector<InputBinding> bindings)
        : _bindings(std::move(bindings))
    {
    }

    const std::vector<InputBinding>& InputMap::getBindings() const noexcept
    {
        return _bindings;
    }

    void InputMap::setBindings(std::vector<InputBinding> bindings)
    {
        _bindings = std::move(bindings);
    }

    bool InputMap::bind(InputBinding binding)
    {
        return Bind(_bindings, std::move(binding));
    }

    bool InputMap::unbind(const Core::StringAtom& action)
    {
        return Unbind(_bindings, action);
    }

    std::optional<InputBinding> InputMap::resolveBinding(const KeyInputEvent& event) const
    {
        return ResolveBinding(_bindings, event);
    }
} // namespace NX
