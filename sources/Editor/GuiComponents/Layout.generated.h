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
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 5; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::optional<float>", "_height" },
			{ "std::optional<float>", "_width" },
			{ "glm::vec4", "_paddings" },
			{ "Core::Gui::Align", "_align" },
			{ "Core::Gui::Align", "_secondAlign" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::Layout& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
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
        return s;
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
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::Layout& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gui::Layout>(obj);
        }

		R<Core::Gui::Widget>::Deserialize<RImpl>(s, obj, true);
		s.read("_height", obj._height, 4);
		s.read("_width", obj._width, 4);
		s.read("_paddings", obj._paddings, glm::vec4{});
		s.read("_align", obj._align, Core::Gui::Align::None);
		s.read("_secondAlign", obj._secondAlign, Core::Gui::Align::None);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gui::Layout>(obj, s.logs());
        }
    }
}; // struct R<Core::Gui::Layout>
// clang-format on
