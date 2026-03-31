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
struct R<Core::Gizmo::Direction>
{
    static constexpr std::string_view Name() { return "Direction"; }
    static constexpr std::size_t Size() { return 3; }
    static constexpr std::string_view ParentScope() { return "Core::Gizmo"; }

    static std::string_view ToString(::Core::Gizmo::Direction value)
    {
        const auto& data = R<Core::Gizmo::Direction>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::Gizmo::Direction> FromString(std::string_view value)
    {
        const auto& data = R<Core::Gizmo::Direction>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::Gizmo::Direction, 3>& ToArrayC()
    {
        static constexpr std::array<::Core::Gizmo::Direction, 3> constants = {
				::Core::Gizmo::Direction::X,
				::Core::Gizmo::Direction::Y,
				::Core::Gizmo::Direction::Z
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 3>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 3> names = {
				std::string_view("X"),
				std::string_view("Y"),
				std::string_view("Z")
        };

        return names;
    }

    static const std::unordered_map<::Core::Gizmo::Direction, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::Gizmo::Direction, std::string_view> map = {
				{ ::Core::Gizmo::Direction::X, "X" },
				{ ::Core::Gizmo::Direction::Y, "Y" },
				{ ::Core::Gizmo::Direction::Z, "Z" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::Gizmo::Direction>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::Gizmo::Direction> map = {
				{ "X", ::Core::Gizmo::Direction::X },
				{ "Y", ::Core::Gizmo::Direction::Y },
				{ "Z", ::Core::Gizmo::Direction::Z }
        };

        return map;
    }
}; // struct R<Core::Gizmo::Direction>

// clang-format on
