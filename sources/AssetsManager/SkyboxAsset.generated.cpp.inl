/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

namespace R
{

    namespace Core::SkyboxAsset::Direction
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "Direction"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::SkyboxAsset"; return name; }

        const std::string& ToString(::Core::SkyboxAsset::Direction value)
        {
            const auto& data = R::Core::SkyboxAsset::Direction::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static constexpr std::string empty{};
            return empty;
        }

        std::optional<::Core::SkyboxAsset::Direction> FromString(const std::string& value)
        {
            const auto& data = R::Core::SkyboxAsset::Direction::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::SkyboxAsset::Direction, 6>& ToArrayC()
        {
            static const std::array<::Core::SkyboxAsset::Direction, 6> constants = {
				::Core::SkyboxAsset::Direction::Right,
				::Core::SkyboxAsset::Direction::Left,
				::Core::SkyboxAsset::Direction::Top,
				::Core::SkyboxAsset::Direction::Bottom,
				::Core::SkyboxAsset::Direction::Front,
				::Core::SkyboxAsset::Direction::Back
            };

            return constants;
        }

        const std::array<std::string, 6>& ToArrayN()
        {
            static const std::array<std::string, 6> names = {
				std::string("Right"),
				std::string("Left"),
				std::string("Top"),
				std::string("Bottom"),
				std::string("Front"),
				std::string("Back")
            };

            return names;
        }

        const std::unordered_map<::Core::SkyboxAsset::Direction, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::SkyboxAsset::Direction, std::string> map = {
				{ ::Core::SkyboxAsset::Direction::Right, "Right" },
				{ ::Core::SkyboxAsset::Direction::Left, "Left" },
				{ ::Core::SkyboxAsset::Direction::Top, "Top" },
				{ ::Core::SkyboxAsset::Direction::Bottom, "Bottom" },
				{ ::Core::SkyboxAsset::Direction::Front, "Front" },
				{ ::Core::SkyboxAsset::Direction::Back, "Back" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::SkyboxAsset::Direction>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::SkyboxAsset::Direction> map = {
				{ "Right", ::Core::SkyboxAsset::Direction::Right },
				{ "Left", ::Core::SkyboxAsset::Direction::Left },
				{ "Top", ::Core::SkyboxAsset::Direction::Top },
				{ "Bottom", ::Core::SkyboxAsset::Direction::Bottom },
				{ "Front", ::Core::SkyboxAsset::Direction::Front },
				{ "Back", ::Core::SkyboxAsset::Direction::Back }
            };

            return map;
        }

    } // namespace Core::SkyboxAsset::Direction

    const std::string& ToString(::Core::SkyboxAsset::Direction value)
    {
        return Core::SkyboxAsset::Direction::ToString(value);
    }


} // namespace
