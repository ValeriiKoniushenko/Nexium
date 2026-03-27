/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

#include <array>
#include <optional>
#include <string>
#include <unordered_map>

template<>
struct R<Core::ViewportMode>
{
    static constexpr std::string_view Name() { return "ViewportMode"; }
    static constexpr std::size_t Size() { return 2; }
    static constexpr std::string_view ParentScope() { return "Core"; }

    static std::string_view ToString(::Core::ViewportMode value)
    {
        const auto& data = R<Core::ViewportMode>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::ViewportMode> FromString(std::string_view value)
    {
        const auto& data = R<Core::ViewportMode>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::ViewportMode, 2>& ToArrayC()
    {
        static constexpr std::array<::Core::ViewportMode, 2> constants
            = { ::Core::ViewportMode::Default, ::Core::ViewportMode::ZoomIn };

        return constants;
    }

    static constexpr const std::array<std::string_view, 2>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 2> names
            = { std::string_view("Default"), std::string_view("ZoomIn") };

        return names;
    }

    static const std::unordered_map<::Core::ViewportMode, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::ViewportMode, std::string_view> map
            = { { ::Core::ViewportMode::Default, "Default" },
                { ::Core::ViewportMode::ZoomIn, "ZoomIn" } };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::ViewportMode>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::ViewportMode> map
            = { { "Default", ::Core::ViewportMode::Default },
                { "ZoomIn", ::Core::ViewportMode::ZoomIn } };

        return map;
    }
}; // struct R<Core::ViewportMode>
