// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Editor

#pragma once

#include "Combo.h"
#include "Core/Delegate.h"

#include <cstdint>
#include <functional>
#include <string_view>
#include <string>
#include <type_traits>
#include <vector>

namespace NX::Gui
{
    /// One enumerator, type-erased. `value` holds the underlying value's bits, so any enum
    /// (signed or not, up to 64 bits) round-trips through `static_cast`.
    struct EnumOption
    {
        Core::StringAtom name;
        std::uint64_t value = 0;
    };

    template<class E>
    concept IsReflectedEnum = std::is_enum_v<E> && requires {
        R<E>::ToArrayC();
        R<E>::ToArrayN();
    };

    template<class E>
        requires std::is_enum_v<E>
    [[nodiscard]] constexpr std::uint64_t EnumBits(E value) noexcept
    {
        return static_cast<std::uint64_t>(static_cast<std::underlying_type_t<E>>(value));
    }

    /// The enumerators of a JRM-reflected enum, in declaration order.
    template<IsReflectedEnum E>
    [[nodiscard]] std::vector<EnumOption> EnumOptionsOf()
    {
        const auto& values = R<E>::ToArrayC();
        const auto& names = R<E>::ToArrayN();
        std::vector<EnumOption> out;
        out.reserve(values.size());
        for (std::size_t i = 0; i < values.size(); ++i)
        {
            out.push_back({ .name = Core::StringAtom::Intern(names[i]),
                            .value = EnumBits(values[i]) });
        }
        return out;
    }

    /// A combo box choosing one enumerator. A value that matches no option is shown as
    /// `<number>` and left untouched until the user picks something.
    CLASS();
    class EnumCombo : public BaseCombo
    {
        ECS_DECL(EnumCombo, NX::Gui::BaseCombo);

    public:
        void setOptions(std::vector<EnumOption> options);
        [[nodiscard]] const std::vector<EnumOption>& getOptions() const noexcept
        {
            return _options;
        }

        /// Pull: shows `value`, does not notify.
        void setValue(std::uint64_t value);
        [[nodiscard]] std::uint64_t getValue() const noexcept { return _value; }

        [[nodiscard]] const std::string& getPreviewText() const noexcept { return _preview; }

    public: // Delegates
        Delegate<void(std::uint64_t)>::Ptr onSelect = Delegate<void(std::uint64_t)>::Create();

    protected:
        void onDraw() override;
        void drawPopupContent() override;

        /// The user picked `_options[index]`.
        void select(std::size_t index);

        void updatePreview();

    protected:
        std::vector<EnumOption> _options;
        std::uint64_t _value = 0;
        std::string _preview;
    };

    /// A combo box with one checkbox per single-bit enumerator. Zero and combined
    /// enumerators are not listed; bits that no enumerator names are kept as they are.
    CLASS();
    class FlagsEditor : public BaseCombo
    {
        ECS_DECL(FlagsEditor, NX::Gui::BaseCombo);

    public:
        void setOptions(std::vector<EnumOption> options);

        /// The enumerators that get a checkbox, in declaration order.
        [[nodiscard]] const std::vector<EnumOption>& getFlagOptions() const noexcept
        {
            return _flags;
        }

        /// Pull: shows `value`, does not notify.
        void setValue(std::uint64_t value);
        [[nodiscard]] std::uint64_t getValue() const noexcept { return _value; }

        [[nodiscard]] const std::string& getPreviewText() const noexcept { return _preview; }

    public: // Delegates
        Delegate<void(std::uint64_t)>::Ptr onChange = Delegate<void(std::uint64_t)>::Create();

    protected:
        void onDraw() override;
        void drawPopupContent() override;

        /// The user flipped `getFlagOptions()[index]`.
        void toggle(std::size_t index);

        void updatePreview();

    protected:
        std::vector<EnumOption> _flags;
        Core::StringAtom _emptyName = "None"_atom;
        std::uint64_t _value = 0;
        std::string _preview;
    };

    /// A combo box choosing one string from a list that is fetched when the popup opens (asset
    /// names, animation names, ...), so the list is always current and costs nothing while
    /// closed. The current value is shown even when it is not one of the options.
    CLASS();
    class ChoiceCombo : public BaseCombo
    {
        ECS_DECL(ChoiceCombo, NX::Gui::BaseCombo);

    public:
        using OptionsProvider = std::function<std::vector<Core::StringAtom>()>;

        void setOptionsProvider(OptionsProvider provider) { _provider = std::move(provider); }

        /// Asks the provider now and keeps the result.
        const std::vector<Core::StringAtom>& refreshOptions();
        [[nodiscard]] const std::vector<Core::StringAtom>& getOptions() const noexcept
        {
            return _options;
        }

        /// Pull: shows `value`, does not notify.
        void setValue(Core::StringAtom value);
        [[nodiscard]] const Core::StringAtom& getValue() const noexcept { return _value; }

        [[nodiscard]] std::string_view getPreviewText() const noexcept;

    public: // Delegates
        Delegate<void(const Core::StringAtom&)>::Ptr onSelect
            = Delegate<void(const Core::StringAtom&)>::Create();

    protected:
        void onDraw() override;
        void drawPopupContent() override;

        /// The user picked `getOptions()[index]`.
        void select(std::size_t index);

    protected:
        OptionsProvider _provider;
        std::vector<Core::StringAtom> _options;
        Core::StringAtom _value;
        bool _wasOpen = false;
    };
} // namespace NX::Gui

#include "EnumInputs.generated.h" // added by the code generator. Better don't move it.
