/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

// clang-format off
#include <array>
#include <optional>
#include <string>
#include <unordered_map>

template<>
struct R<Core::InputActionTrigger>
{
    static constexpr std::string_view Name() { return "InputActionTrigger"; }
    static constexpr std::string_view FullName() { return "Core::InputActionTrigger"; }
    static constexpr std::size_t Size() { return 3; }
    static constexpr std::string_view ParentScope() { return "Core"; }

    static std::string_view ToString(::Core::InputActionTrigger value)
    {
        const auto& data = R<Core::InputActionTrigger>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static constexpr std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::InputActionTrigger> FromString(std::string_view value)
    {
        const auto& data = R<Core::InputActionTrigger>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(::Core::InputActionTrigger value, RResourceStream<RImpl>& s)
    {
        s.getData() = ToString(value);
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static RResourceStream<RImpl> Serialize(::Core::InputActionTrigger value)
    {
        RResourceStream<RImpl> s;
        Serialize<RImpl>(value, s);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, ::Core::InputActionTrigger& value)
    {
        auto tmp = FromString(s.getData().template get<std::string>());
        if (tmp.has_value())
            value = tmp.value();
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static ::Core::InputActionTrigger Deserialize(const RResourceStream<RImpl>& s)
    {
        ::Core::InputActionTrigger out;
        Deserialize<RImpl>(s, out);
        return out;
    }

    static constexpr const std::array<::Core::InputActionTrigger, 3>& ToArrayC()
    {
        static constexpr std::array<::Core::InputActionTrigger, 3> constants = {
				::Core::InputActionTrigger::WhileHeld,
				::Core::InputActionTrigger::OnPress,
				::Core::InputActionTrigger::OnRelease
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 3>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 3> names = {
				std::string_view("WhileHeld"),
				std::string_view("OnPress"),
				std::string_view("OnRelease")
        };

        return names;
    }

    static const std::unordered_map<::Core::InputActionTrigger, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::InputActionTrigger, std::string_view> map = {
				{ ::Core::InputActionTrigger::WhileHeld, "WhileHeld" },
				{ ::Core::InputActionTrigger::OnPress, "OnPress" },
				{ ::Core::InputActionTrigger::OnRelease, "OnRelease" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::InputActionTrigger>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::InputActionTrigger> map = {
				{ "WhileHeld", ::Core::InputActionTrigger::WhileHeld },
				{ "OnPress", ::Core::InputActionTrigger::OnPress },
				{ "OnRelease", ::Core::InputActionTrigger::OnRelease }
        };

        return map;
    }
}; // struct R<Core::InputActionTrigger>

template<>
struct R<Core::InputModifier>
{
    static constexpr std::string_view Name() { return "InputModifier"; }
    static constexpr std::string_view FullName() { return "Core::InputModifier"; }
    static constexpr std::size_t Size() { return 6; }
    static constexpr std::string_view ParentScope() { return "Core"; }

    static std::string_view ToString(::Core::InputModifier value)
    {
        const auto& data = R<Core::InputModifier>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static constexpr std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::InputModifier> FromString(std::string_view value)
    {
        const auto& data = R<Core::InputModifier>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(::Core::InputModifier value, RResourceStream<RImpl>& s)
    {
        s.getData() = ToString(value);
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static RResourceStream<RImpl> Serialize(::Core::InputModifier value)
    {
        RResourceStream<RImpl> s;
        Serialize<RImpl>(value, s);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, ::Core::InputModifier& value)
    {
        auto tmp = FromString(s.getData().template get<std::string>());
        if (tmp.has_value())
            value = tmp.value();
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static ::Core::InputModifier Deserialize(const RResourceStream<RImpl>& s)
    {
        ::Core::InputModifier out;
        Deserialize<RImpl>(s, out);
        return out;
    }

    static constexpr const std::array<::Core::InputModifier, 6>& ToArrayC()
    {
        static constexpr std::array<::Core::InputModifier, 6> constants = {
				::Core::InputModifier::None,
				::Core::InputModifier::Shift,
				::Core::InputModifier::Control,
				::Core::InputModifier::Alt,
				::Core::InputModifier::Super,
				::Core::InputModifier::All
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 6>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 6> names = {
				std::string_view("None"),
				std::string_view("Shift"),
				std::string_view("Control"),
				std::string_view("Alt"),
				std::string_view("Super"),
				std::string_view("All")
        };

        return names;
    }

    static const std::unordered_map<::Core::InputModifier, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::InputModifier, std::string_view> map = {
				{ ::Core::InputModifier::None, "None" },
				{ ::Core::InputModifier::Shift, "Shift" },
				{ ::Core::InputModifier::Control, "Control" },
				{ ::Core::InputModifier::Alt, "Alt" },
				{ ::Core::InputModifier::Super, "Super" },
				{ ::Core::InputModifier::All, "All" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::InputModifier>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::InputModifier> map = {
				{ "None", ::Core::InputModifier::None },
				{ "Shift", ::Core::InputModifier::Shift },
				{ "Control", ::Core::InputModifier::Control },
				{ "Alt", ::Core::InputModifier::Alt },
				{ "Super", ::Core::InputModifier::Super },
				{ "All", ::Core::InputModifier::All }
        };

        return map;
    }
}; // struct R<Core::InputModifier>

template<>
struct R<Core::InputContext>
{
    static constexpr std::string_view Name() { return "InputContext"; }
    static constexpr std::string_view FullName() { return "Core::InputContext"; }
    static constexpr std::size_t Size() { return 2; }
    static constexpr std::string_view ParentScope() { return "Core"; }

    static std::string_view ToString(::Core::InputContext value)
    {
        const auto& data = R<Core::InputContext>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static constexpr std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::InputContext> FromString(std::string_view value)
    {
        const auto& data = R<Core::InputContext>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(::Core::InputContext value, RResourceStream<RImpl>& s)
    {
        s.getData() = ToString(value);
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static RResourceStream<RImpl> Serialize(::Core::InputContext value)
    {
        RResourceStream<RImpl> s;
        Serialize<RImpl>(value, s);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, ::Core::InputContext& value)
    {
        auto tmp = FromString(s.getData().template get<std::string>());
        if (tmp.has_value())
            value = tmp.value();
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static ::Core::InputContext Deserialize(const RResourceStream<RImpl>& s)
    {
        ::Core::InputContext out;
        Deserialize<RImpl>(s, out);
        return out;
    }

    static constexpr const std::array<::Core::InputContext, 2>& ToArrayC()
    {
        static constexpr std::array<::Core::InputContext, 2> constants = {
				::Core::InputContext::Editor,
				::Core::InputContext::Gameplay
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 2>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 2> names = {
				std::string_view("Editor"),
				std::string_view("Gameplay")
        };

        return names;
    }

    static const std::unordered_map<::Core::InputContext, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::InputContext, std::string_view> map = {
				{ ::Core::InputContext::Editor, "Editor" },
				{ ::Core::InputContext::Gameplay, "Gameplay" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::InputContext>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::InputContext> map = {
				{ "Editor", ::Core::InputContext::Editor },
				{ "Gameplay", ::Core::InputContext::Gameplay }
        };

        return map;
    }
}; // struct R<Core::InputContext>

// clang-format on
