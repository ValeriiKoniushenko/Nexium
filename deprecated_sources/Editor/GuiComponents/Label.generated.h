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
    static constexpr std::string_view FullName() { return "Core::Gui::Label"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 6; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::optional<Core::Color4>", "_textColor", {} },
			{ "Core::Gui::Align", "_align", {} },
			{ "glm::vec2", "_textSize", {} },
			{ "float", "_width", {} },
			{ "float", "_height", {} },
			{ "bool", "_isTruncateLongText", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
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
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::Label& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::Label& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gui::Label>(obj);
        }

		R<Core::Gui::Widget>::Deserialize<RImpl>(s, obj, true);
		s.read("_textColor", obj._textColor, decltype(obj._textColor){}, 16);
		s.read("_align", obj._align, Core::Gui::Align::Left, 16);
		s.read("_textSize", obj._textSize, glm::vec2{}, 16);
		s.read("_width", obj._width, -1.0f, 16);
		s.read("_height", obj._height, -1.0f, 16);
		s.read("_isTruncateLongText", obj._isTruncateLongText, true, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gui::Label>(obj, s.logs());
        }
    }

    static bool GetField(Core::Gui::Label& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_textColor") == 0)
        {
            onFound((&obj._textColor), "std::optional<Core::Color4>");
            return true;
        }
        if (std::strcmp(fieldName, "_align") == 0)
        {
            onFound((&obj._align), "Core::Gui::Align");
            return true;
        }
        if (std::strcmp(fieldName, "_textSize") == 0)
        {
            onFound((&obj._textSize), "glm::vec2");
            return true;
        }
        if (std::strcmp(fieldName, "_width") == 0)
        {
            onFound((&obj._width), "float");
            return true;
        }
        if (std::strcmp(fieldName, "_height") == 0)
        {
            onFound((&obj._height), "float");
            return true;
        }
        if (std::strcmp(fieldName, "_isTruncateLongText") == 0)
        {
            onFound((&obj._isTruncateLongText), "bool");
            return true;
        }

		return false;
    }
}; // struct R<Core::Gui::Label>
// clang-format on
