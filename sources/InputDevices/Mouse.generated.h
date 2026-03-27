/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

// clang-format off#include <array>
#include <optional>
#include <string>
#include <unordered_map>

template<>
struct R<Core::Mouse::State>
{
    static constexpr std::string_view Name() { return "State"; }
    static constexpr std::size_t Size() { return 3; }
    static constexpr std::string_view ParentScope() { return "Core::Mouse"; }

    static std::string_view ToString(::Core::Mouse::State value)
    {
        const auto& data = R<Core::Mouse::State>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::Mouse::State> FromString(std::string_view value)
    {
        const auto& data = R<Core::Mouse::State>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::Mouse::State, 3>& ToArrayC()
    {
        static constexpr std::array<::Core::Mouse::State, 3> constants
            = { ::Core::Mouse::State::None, ::Core::Mouse::State::Press,
                ::Core::Mouse::State::Release };

        return constants;
    }

    static constexpr const std::array<std::string_view, 3>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 3> names
            = { std::string_view("None"), std::string_view("Press"), std::string_view("Release") };

        return names;
    }

    static const std::unordered_map<::Core::Mouse::State, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::Mouse::State, std::string_view> map
            = { { ::Core::Mouse::State::None, "None" },
                { ::Core::Mouse::State::Press, "Press" },
                { ::Core::Mouse::State::Release, "Release" } };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::Mouse::State>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::Mouse::State> map
            = { { "None", ::Core::Mouse::State::None },
                { "Press", ::Core::Mouse::State::Press },
                { "Release", ::Core::Mouse::State::Release } };

        return map;
    }
}; // struct R<Core::Mouse::State>

template<>
struct R<Core::Mouse::Key>
{
    static constexpr std::string_view Name() { return "Key"; }
    static constexpr std::size_t Size() { return 13; }
    static constexpr std::string_view ParentScope() { return "Core::Mouse"; }

    static std::string_view ToString(::Core::Mouse::Key value)
    {
        const auto& data = R<Core::Mouse::Key>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::Mouse::Key> FromString(std::string_view value)
    {
        const auto& data = R<Core::Mouse::Key>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::Mouse::Key, 13>& ToArrayC()
    {
        static constexpr std::array<::Core::Mouse::Key, 13> constants
            = { ::Core::Mouse::Key::_1,   ::Core::Mouse::Key::_2,    ::Core::Mouse::Key::_3,
                ::Core::Mouse::Key::_4,   ::Core::Mouse::Key::_5,    ::Core::Mouse::Key::_6,
                ::Core::Mouse::Key::_7,   ::Core::Mouse::Key::_8,    ::Core::Mouse::Key::Last,
                ::Core::Mouse::Key::Left, ::Core::Mouse::Key::Right, ::Core::Mouse::Key::Middle,
                ::Core::Mouse::Key::None };

        return constants;
    }

    static constexpr const std::array<std::string_view, 13>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 13> names
            = { std::string_view("_1"),   std::string_view("_2"),    std::string_view("_3"),
                std::string_view("_4"),   std::string_view("_5"),    std::string_view("_6"),
                std::string_view("_7"),   std::string_view("_8"),    std::string_view("Last"),
                std::string_view("Left"), std::string_view("Right"), std::string_view("Middle"),
                std::string_view("None") };

        return names;
    }

    static const std::unordered_map<::Core::Mouse::Key, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::Mouse::Key, std::string_view> map
            = { { ::Core::Mouse::Key::_1, "_1" },       { ::Core::Mouse::Key::_2, "_2" },
                { ::Core::Mouse::Key::_3, "_3" },       { ::Core::Mouse::Key::_4, "_4" },
                { ::Core::Mouse::Key::_5, "_5" },       { ::Core::Mouse::Key::_6, "_6" },
                { ::Core::Mouse::Key::_7, "_7" },       { ::Core::Mouse::Key::_8, "_8" },
                { ::Core::Mouse::Key::Last, "Last" },   { ::Core::Mouse::Key::Left, "Left" },
                { ::Core::Mouse::Key::Right, "Right" }, { ::Core::Mouse::Key::Middle, "Middle" },
                { ::Core::Mouse::Key::None, "None" } };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::Mouse::Key>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::Mouse::Key> map
            = { { "_1", ::Core::Mouse::Key::_1 },       { "_2", ::Core::Mouse::Key::_2 },
                { "_3", ::Core::Mouse::Key::_3 },       { "_4", ::Core::Mouse::Key::_4 },
                { "_5", ::Core::Mouse::Key::_5 },       { "_6", ::Core::Mouse::Key::_6 },
                { "_7", ::Core::Mouse::Key::_7 },       { "_8", ::Core::Mouse::Key::_8 },
                { "Last", ::Core::Mouse::Key::Last },   { "Left", ::Core::Mouse::Key::Left },
                { "Right", ::Core::Mouse::Key::Right }, { "Middle", ::Core::Mouse::Key::Middle },
                { "None", ::Core::Mouse::Key::None } };

        return map;
    }
}; // struct R<Core::Mouse::Key>

template<>
struct R<Core::Mouse::Mod>
{
    static constexpr std::string_view Name() { return "Mod"; }
    static constexpr std::size_t Size() { return 7; }
    static constexpr std::string_view ParentScope() { return "Core::Mouse"; }

    static std::string_view ToString(::Core::Mouse::Mod value)
    {
        const auto& data = R<Core::Mouse::Mod>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::Mouse::Mod> FromString(std::string_view value)
    {
        const auto& data = R<Core::Mouse::Mod>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::Mouse::Mod, 7>& ToArrayC()
    {
        static constexpr std::array<::Core::Mouse::Mod, 7> constants
            = { ::Core::Mouse::Mod::None,   ::Core::Mouse::Mod::Shift, ::Core::Mouse::Mod::Control,
                ::Core::Mouse::Mod::Alt,    ::Core::Mouse::Mod::Super, ::Core::Mouse::Mod::CapsLock,
                ::Core::Mouse::Mod::NumLock };

        return constants;
    }

    static constexpr const std::array<std::string_view, 7>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 7> names
            = { std::string_view("None"),   std::string_view("Shift"), std::string_view("Control"),
                std::string_view("Alt"),    std::string_view("Super"), std::string_view("CapsLock"),
                std::string_view("NumLock") };

        return names;
    }

    static const std::unordered_map<::Core::Mouse::Mod, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::Mouse::Mod, std::string_view> map
            = { { ::Core::Mouse::Mod::None, "None" },
                { ::Core::Mouse::Mod::Shift, "Shift" },
                { ::Core::Mouse::Mod::Control, "Control" },
                { ::Core::Mouse::Mod::Alt, "Alt" },
                { ::Core::Mouse::Mod::Super, "Super" },
                { ::Core::Mouse::Mod::CapsLock, "CapsLock" },
                { ::Core::Mouse::Mod::NumLock, "NumLock" } };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::Mouse::Mod>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::Mouse::Mod> map
            = { { "None", ::Core::Mouse::Mod::None },
                { "Shift", ::Core::Mouse::Mod::Shift },
                { "Control", ::Core::Mouse::Mod::Control },
                { "Alt", ::Core::Mouse::Mod::Alt },
                { "Super", ::Core::Mouse::Mod::Super },
                { "CapsLock", ::Core::Mouse::Mod::CapsLock },
                { "NumLock", ::Core::Mouse::Mod::NumLock } };

        return map;
    }
}; // struct R<Core::Mouse::Mod>

// clang-format on