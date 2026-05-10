/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

// clang-format off
#include <string>
#include <string_view>
#include <vector>

template<>
struct R<Core::Gui::Button>
{
    static constexpr std::string_view Name() { return "Button"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 10; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::optional<Color4>", "_buttonColor" },
			{ "std::optional<Color4>", "_buttonHoverColor" },
			{ "std::optional<Color4>", "_buttonActiveColor" },
			{ "std::optional<Color4>", "_textColor" },
			{ "std::optional<Color4>", "_borderColor" },
			{ "std::optional<float>", "_borderRound" },
			{ "std::optional<float>", "_borderWidth" },
			{ "glm::vec2", "_textSize" },
			{ "glm::vec2", "_size" },
			{ "glm::vec2", "_minSize" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::Button& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::Button>(obj);
        }

		s.write(R<Core::Gui::Widget>::Serialize<RImpl>(obj).getData());
		s.write("_buttonColor", obj._buttonColor);
		s.write("_buttonHoverColor", obj._buttonHoverColor);
		s.write("_buttonActiveColor", obj._buttonActiveColor);
		s.write("_textColor", obj._textColor);
		s.write("_borderColor", obj._borderColor);
		s.write("_borderRound", obj._borderRound);
		s.write("_borderWidth", obj._borderWidth);
		s.write("_textSize", obj._textSize);
		s.write("_size", obj._size);
		s.write("_minSize", obj._minSize);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::Button>(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::Button& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::Button>(obj);
        }

		s.write(R<Core::Gui::Widget>::Serialize<RImpl>(obj).getData());
		s.write("_buttonColor", obj._buttonColor);
		s.write("_buttonHoverColor", obj._buttonHoverColor);
		s.write("_buttonActiveColor", obj._buttonActiveColor);
		s.write("_textColor", obj._textColor);
		s.write("_borderColor", obj._borderColor);
		s.write("_borderRound", obj._borderRound);
		s.write("_borderWidth", obj._borderWidth);
		s.write("_textSize", obj._textSize);
		s.write("_size", obj._size);
		s.write("_minSize", obj._minSize);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::Button>(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::Button& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gui::Button>(obj);
        }

		R<Core::Gui::Widget>::Deserialize<RImpl>(s, obj, true);
		s.read("_buttonColor", obj._buttonColor, 4);
		s.read("_buttonHoverColor", obj._buttonHoverColor, 4);
		s.read("_buttonActiveColor", obj._buttonActiveColor, 4);
		s.read("_textColor", obj._textColor, 4);
		s.read("_borderColor", obj._borderColor, 4);
		s.read("_borderRound", obj._borderRound, 4);
		s.read("_borderWidth", obj._borderWidth, 4);
		s.read("_textSize", obj._textSize, glm::vec2{});
		s.read("_size", obj._size, glm::vec2{});
		s.read("_minSize", obj._minSize, glm::vec2{});
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gui::Button>(obj, s.logs());
        }
    }
}; // struct R<Core::Gui::Button>

template<>
struct R<Core::Gui::ToggleButton>
{
    static constexpr std::string_view Name() { return "ToggleButton"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "bool", "_isActive" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::ToggleButton& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::ToggleButton>(obj);
        }

		s.write(R<Core::Gui::Button>::Serialize<RImpl>(obj).getData());
		s.write("_isActive", obj._isActive);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::ToggleButton>(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::ToggleButton& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::ToggleButton>(obj);
        }

		s.write(R<Core::Gui::Button>::Serialize<RImpl>(obj).getData());
		s.write("_isActive", obj._isActive);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::ToggleButton>(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::ToggleButton& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gui::ToggleButton>(obj);
        }

		R<Core::Gui::Button>::Deserialize<RImpl>(s, obj, true);
		s.read("_isActive", obj._isActive, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gui::ToggleButton>(obj, s.logs());
        }
    }
}; // struct R<Core::Gui::ToggleButton>

template<>
struct R<Core::Gui::ImageButton>
{
    static constexpr std::string_view Name() { return "ImageButton"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::optional<glm::vec2>", "_paddingSize" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::ImageButton& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::ImageButton>(obj);
        }

		s.write(R<Core::Gui::Button>::Serialize<RImpl>(obj).getData());
		s.write("_paddingSize", obj._paddingSize);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::ImageButton>(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::ImageButton& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::ImageButton>(obj);
        }

		s.write(R<Core::Gui::Button>::Serialize<RImpl>(obj).getData());
		s.write("_paddingSize", obj._paddingSize);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::ImageButton>(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::ImageButton& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gui::ImageButton>(obj);
        }

		R<Core::Gui::Button>::Deserialize<RImpl>(s, obj, true);
		s.read("_paddingSize", obj._paddingSize, 4);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gui::ImageButton>(obj, s.logs());
        }
    }
}; // struct R<Core::Gui::ImageButton>
// clang-format on
