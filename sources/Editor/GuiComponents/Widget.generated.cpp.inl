/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

namespace R
{

    namespace Core::Gui::Flex
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "Flex"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::Gui"; return name; }

        const std::string& ToString(::Core::Gui::Flex value)
        {
            const auto& data = R::Core::Gui::Flex::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static const std::string empty{};
            return empty;
        }

        std::optional<::Core::Gui::Flex> FromString(const std::string& value)
        {
            const auto& data = R::Core::Gui::Flex::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::Gui::Flex, 5>& ToArrayC()
        {
            static const std::array<::Core::Gui::Flex, 5> constants = {
				::Core::Gui::Flex::None,
				::Core::Gui::Flex::Fixed,
				::Core::Gui::Flex::FlexWidth,
				::Core::Gui::Flex::FlexHeight,
				::Core::Gui::Flex::FlexWidthAndHeight
            };

            return constants;
        }

        const std::array<std::string, 5>& ToArrayN()
        {
            static const std::array<std::string, 5> names = {
				std::string("None"),
				std::string("Fixed"),
				std::string("FlexWidth"),
				std::string("FlexHeight"),
				std::string("FlexWidthAndHeight")
            };

            return names;
        }

        const std::unordered_map<::Core::Gui::Flex, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::Gui::Flex, std::string> map = {
				{ ::Core::Gui::Flex::None, "None" },
				{ ::Core::Gui::Flex::Fixed, "Fixed" },
				{ ::Core::Gui::Flex::FlexWidth, "FlexWidth" },
				{ ::Core::Gui::Flex::FlexHeight, "FlexHeight" },
				{ ::Core::Gui::Flex::FlexWidthAndHeight, "FlexWidthAndHeight" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::Gui::Flex>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::Gui::Flex> map = {
				{ "None", ::Core::Gui::Flex::None },
				{ "Fixed", ::Core::Gui::Flex::Fixed },
				{ "FlexWidth", ::Core::Gui::Flex::FlexWidth },
				{ "FlexHeight", ::Core::Gui::Flex::FlexHeight },
				{ "FlexWidthAndHeight", ::Core::Gui::Flex::FlexWidthAndHeight }
            };

            return map;
        }

    } // namespace Core::Gui::Flex

    const std::string& ToString(::Core::Gui::Flex value)
    {
        return Core::Gui::Flex::ToString(value);
    }

    namespace Core::Gui::Align
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "Align"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::Gui"; return name; }

        const std::string& ToString(::Core::Gui::Align value)
        {
            const auto& data = R::Core::Gui::Align::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static const std::string empty{};
            return empty;
        }

        std::optional<::Core::Gui::Align> FromString(const std::string& value)
        {
            const auto& data = R::Core::Gui::Align::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::Gui::Align, 7>& ToArrayC()
        {
            static const std::array<::Core::Gui::Align, 7> constants = {
				::Core::Gui::Align::None,
				::Core::Gui::Align::Left,
				::Core::Gui::Align::Right,
				::Core::Gui::Align::SpaceBetween,
				::Core::Gui::Align::Center,
				::Core::Gui::Align::Top,
				::Core::Gui::Align::Bottom
            };

            return constants;
        }

        const std::array<std::string, 7>& ToArrayN()
        {
            static const std::array<std::string, 7> names = {
				std::string("None"),
				std::string("Left"),
				std::string("Right"),
				std::string("SpaceBetween"),
				std::string("Center"),
				std::string("Top"),
				std::string("Bottom")
            };

            return names;
        }

        const std::unordered_map<::Core::Gui::Align, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::Gui::Align, std::string> map = {
				{ ::Core::Gui::Align::None, "None" },
				{ ::Core::Gui::Align::Left, "Left" },
				{ ::Core::Gui::Align::Right, "Right" },
				{ ::Core::Gui::Align::SpaceBetween, "SpaceBetween" },
				{ ::Core::Gui::Align::Center, "Center" },
				{ ::Core::Gui::Align::Top, "Top" },
				{ ::Core::Gui::Align::Bottom, "Bottom" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::Gui::Align>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::Gui::Align> map = {
				{ "None", ::Core::Gui::Align::None },
				{ "Left", ::Core::Gui::Align::Left },
				{ "Right", ::Core::Gui::Align::Right },
				{ "SpaceBetween", ::Core::Gui::Align::SpaceBetween },
				{ "Center", ::Core::Gui::Align::Center },
				{ "Top", ::Core::Gui::Align::Top },
				{ "Bottom", ::Core::Gui::Align::Bottom }
            };

            return map;
        }

    } // namespace Core::Gui::Align

    const std::string& ToString(::Core::Gui::Align value)
    {
        return Core::Gui::Align::ToString(value);
    }


} // namespace
