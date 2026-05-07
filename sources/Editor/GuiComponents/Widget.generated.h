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
#include <string_view>
#include <unordered_map>
#include <vector>

template<>
struct R<Core::Gui::Flex>
{
    static constexpr std::string_view Name() { return "Flex"; }
    static constexpr std::size_t Size() { return 5; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }

    static std::string_view ToString(::Core::Gui::Flex value)
    {
        const auto& data = R<Core::Gui::Flex>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::Gui::Flex> FromString(std::string_view value)
    {
        const auto& data = R<Core::Gui::Flex>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::Gui::Flex, 5>& ToArrayC()
    {
        static constexpr std::array<::Core::Gui::Flex, 5> constants = {
				::Core::Gui::Flex::None,
				::Core::Gui::Flex::Fixed,
				::Core::Gui::Flex::FlexWidth,
				::Core::Gui::Flex::FlexHeight,
				::Core::Gui::Flex::FlexWidthAndHeight
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 5>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 5> names = {
				std::string_view("None"),
				std::string_view("Fixed"),
				std::string_view("FlexWidth"),
				std::string_view("FlexHeight"),
				std::string_view("FlexWidthAndHeight")
        };

        return names;
    }

    static const std::unordered_map<::Core::Gui::Flex, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::Gui::Flex, std::string_view> map = {
				{ ::Core::Gui::Flex::None, "None" },
				{ ::Core::Gui::Flex::Fixed, "Fixed" },
				{ ::Core::Gui::Flex::FlexWidth, "FlexWidth" },
				{ ::Core::Gui::Flex::FlexHeight, "FlexHeight" },
				{ ::Core::Gui::Flex::FlexWidthAndHeight, "FlexWidthAndHeight" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::Gui::Flex>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::Gui::Flex> map = {
				{ "None", ::Core::Gui::Flex::None },
				{ "Fixed", ::Core::Gui::Flex::Fixed },
				{ "FlexWidth", ::Core::Gui::Flex::FlexWidth },
				{ "FlexHeight", ::Core::Gui::Flex::FlexHeight },
				{ "FlexWidthAndHeight", ::Core::Gui::Flex::FlexWidthAndHeight }
        };

        return map;
    }
}; // struct R<Core::Gui::Flex>

template<>
struct R<Core::Gui::Align>
{
    static constexpr std::string_view Name() { return "Align"; }
    static constexpr std::size_t Size() { return 7; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }

    static std::string_view ToString(::Core::Gui::Align value)
    {
        const auto& data = R<Core::Gui::Align>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::Gui::Align> FromString(std::string_view value)
    {
        const auto& data = R<Core::Gui::Align>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::Gui::Align, 7>& ToArrayC()
    {
        static constexpr std::array<::Core::Gui::Align, 7> constants = {
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

    static constexpr const std::array<std::string_view, 7>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 7> names = {
				std::string_view("None"),
				std::string_view("Left"),
				std::string_view("Right"),
				std::string_view("SpaceBetween"),
				std::string_view("Center"),
				std::string_view("Top"),
				std::string_view("Bottom")
        };

        return names;
    }

    static const std::unordered_map<::Core::Gui::Align, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::Gui::Align, std::string_view> map = {
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

    static const std::unordered_map<std::string_view, ::Core::Gui::Align>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::Gui::Align> map = {
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
}; // struct R<Core::Gui::Align>

template<>
struct R<Core::Gui::Widget>
{
    static constexpr std::string_view Name() { return "Widget"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 5; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "glm::vec2", "_pos" },
			{ "Core::Gui::Flex", "_flex" },
			{ "bool", "_autoDraw" },
			{ "bool", "_isDrawOutline" },
			{ "bool", "_isDisabledWidget" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::Widget& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write("_pos", obj._pos);
		s.write("_flex", obj._flex);
		s.write("_autoDraw", obj._autoDraw);
		s.write("_isDrawOutline", obj._isDrawOutline);
		s.write("_isDisabledWidget", obj._isDisabledWidget);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::Widget& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write("_pos", obj._pos);
		s.write("_flex", obj._flex);
		s.write("_autoDraw", obj._autoDraw);
		s.write("_isDrawOutline", obj._isDrawOutline);
		s.write("_isDisabledWidget", obj._isDisabledWidget);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::Widget& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj, true);
		s.read("_pos", obj._pos, glm::vec2{ 0.f, 0.f });
		s.read("_flex", obj._flex, Core::Gui::Flex::Fixed);
		s.read("_autoDraw", obj._autoDraw, true);
		s.read("_isDrawOutline", obj._isDrawOutline, false);
		s.read("_isDisabledWidget", obj._isDisabledWidget, false);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Core::Gui::Widget>
// clang-format on
