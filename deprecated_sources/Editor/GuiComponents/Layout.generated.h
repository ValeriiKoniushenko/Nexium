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
struct R<Core::Gui::Layout>
{
    static constexpr std::string_view Name() { return "Layout"; }
    static constexpr std::string_view FullName() { return "Core::Gui::Layout"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 5; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::optional<float>", "_height", {} },
			{ "std::optional<float>", "_width", {} },
			{ "glm::vec4", "_paddings", {} },
			{ "Core::Gui::Align", "_align", {} },
			{ "Core::Gui::Align", "_secondAlign", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::Layout& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::Layout>(obj);
        }

		s.write(R<Core::Gui::Widget>::Serialize<RImpl>(obj).getData());
		s.write("_height", obj._height);
		s.write("_width", obj._width);
		s.write("_paddings", obj._paddings);
		s.write("_align", obj._align);
		s.write("_secondAlign", obj._secondAlign);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::Layout>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::Layout& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::Layout& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gui::Layout>(obj);
        }

		R<Core::Gui::Widget>::Deserialize<RImpl>(s, obj, true);
		s.read("_height", obj._height, decltype(obj._height){}, 16);
		s.read("_width", obj._width, decltype(obj._width){}, 16);
		s.read("_paddings", obj._paddings, glm::vec4{}, 16);
		s.read("_align", obj._align, Core::Gui::Align::None, 16);
		s.read("_secondAlign", obj._secondAlign, Core::Gui::Align::None, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gui::Layout>(obj, s.logs());
        }
    }

    static bool GetField(Core::Gui::Layout& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_height") == 0)
        {
            onFound((&obj._height), "std::optional<float>");
            return true;
        }
        if (std::strcmp(fieldName, "_width") == 0)
        {
            onFound((&obj._width), "std::optional<float>");
            return true;
        }
        if (std::strcmp(fieldName, "_paddings") == 0)
        {
            onFound((&obj._paddings), "glm::vec4");
            return true;
        }
        if (std::strcmp(fieldName, "_align") == 0)
        {
            onFound((&obj._align), "Core::Gui::Align");
            return true;
        }
        if (std::strcmp(fieldName, "_secondAlign") == 0)
        {
            onFound((&obj._secondAlign), "Core::Gui::Align");
            return true;
        }

		return false;
    }
}; // struct R<Core::Gui::Layout>
// clang-format on
