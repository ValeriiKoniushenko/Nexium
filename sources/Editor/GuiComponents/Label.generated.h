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
struct R<Core::Gui::Label>
{
    static constexpr std::string_view Name() { return "Label"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 6; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::optional<Core::Color4>", "_textColor" },
			{ "Core::Gui::Align", "_align" },
			{ "glm::vec2", "_textSize" },
			{ "float", "_width" },
			{ "float", "_height" },
			{ "bool", "_isTruncateLongText" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::Label& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::Label>(obj);
        }

		s.write(R<Core::Gui::Widget>::Serialize<RImpl>(obj).getData());
		s.write("_textColor", obj._textColor);
		s.write("_align", obj._align);
		s.write("_textSize", obj._textSize);
		s.write("_width", obj._width);
		s.write("_height", obj._height);
		s.write("_isTruncateLongText", obj._isTruncateLongText);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::Label>(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::Label& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::Label>(obj);
        }

		s.write(R<Core::Gui::Widget>::Serialize<RImpl>(obj).getData());
		s.write("_textColor", obj._textColor);
		s.write("_align", obj._align);
		s.write("_textSize", obj._textSize);
		s.write("_width", obj._width);
		s.write("_height", obj._height);
		s.write("_isTruncateLongText", obj._isTruncateLongText);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::Label>(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::Label& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gui::Label>(obj);
        }

		R<Core::Gui::Widget>::Deserialize<RImpl>(s, obj, true);
		s.read("_textColor", obj._textColor, 4);
		s.read("_align", obj._align, Core::Gui::Align::Left);
		s.read("_textSize", obj._textSize, glm::vec2{});
		s.read("_width", obj._width, -1.0f);
		s.read("_height", obj._height, -1.0f);
		s.read("_isTruncateLongText", obj._isTruncateLongText, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gui::Label>(obj, s.logs());
        }
    }
}; // struct R<Core::Gui::Label>
// clang-format on
