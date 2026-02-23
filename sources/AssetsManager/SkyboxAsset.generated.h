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
struct R<Core::SkyboxAsset::Direction>
{
    static constexpr std::string_view Name() { return "Direction"; }
    static constexpr std::size_t Size() { return 6; }
    static constexpr std::string_view ParentScope() { return "Core::SkyboxAsset"; }

    static std::string_view ToString(::Core::SkyboxAsset::Direction value)
    {
        const auto& data = R<Core::SkyboxAsset::Direction>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::SkyboxAsset::Direction> FromString(std::string_view value)
    {
        const auto& data = R<Core::SkyboxAsset::Direction>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static consteval const std::array<::Core::SkyboxAsset::Direction, 6>& ToArrayC()
    {
        static constexpr std::array<::Core::SkyboxAsset::Direction, 6> constants = {
				::Core::SkyboxAsset::Direction::Right,
				::Core::SkyboxAsset::Direction::Left,
				::Core::SkyboxAsset::Direction::Top,
				::Core::SkyboxAsset::Direction::Bottom,
				::Core::SkyboxAsset::Direction::Front,
				::Core::SkyboxAsset::Direction::Back
        };

        return constants;
    }

    static consteval const std::array<std::string_view, 6>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 6> names = {
				std::string_view("Right"),
				std::string_view("Left"),
				std::string_view("Top"),
				std::string_view("Bottom"),
				std::string_view("Front"),
				std::string_view("Back")
        };

        return names;
    }

    static const std::unordered_map<::Core::SkyboxAsset::Direction, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::SkyboxAsset::Direction, std::string_view> map = {
				{ ::Core::SkyboxAsset::Direction::Right, "Right" },
				{ ::Core::SkyboxAsset::Direction::Left, "Left" },
				{ ::Core::SkyboxAsset::Direction::Top, "Top" },
				{ ::Core::SkyboxAsset::Direction::Bottom, "Bottom" },
				{ ::Core::SkyboxAsset::Direction::Front, "Front" },
				{ ::Core::SkyboxAsset::Direction::Back, "Back" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::SkyboxAsset::Direction>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::SkyboxAsset::Direction> map = {
				{ "Right", ::Core::SkyboxAsset::Direction::Right },
				{ "Left", ::Core::SkyboxAsset::Direction::Left },
				{ "Top", ::Core::SkyboxAsset::Direction::Top },
				{ "Bottom", ::Core::SkyboxAsset::Direction::Bottom },
				{ "Front", ::Core::SkyboxAsset::Direction::Front },
				{ "Back", ::Core::SkyboxAsset::Direction::Back }
        };

        return map;
    }
}; // struct R<Core::SkyboxAsset::Direction>

