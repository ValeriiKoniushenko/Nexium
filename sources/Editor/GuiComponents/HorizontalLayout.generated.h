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
struct R<Core::Gui::HorizontalLayout>
{
    static constexpr std::string_view Name() { return "HorizontalLayout"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "float", "_spacing" },
			{ "bool", "_fitContent" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::HorizontalLayout& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::HorizontalLayout>(obj);
        }

		s.write(R<Core::Gui::Layout>::Serialize<RImpl>(obj).getData());
		s.write("_spacing", obj._spacing);
		s.write("_fitContent", obj._fitContent);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::HorizontalLayout>(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::HorizontalLayout& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::HorizontalLayout>(obj);
        }

		s.write(R<Core::Gui::Layout>::Serialize<RImpl>(obj).getData());
		s.write("_spacing", obj._spacing);
		s.write("_fitContent", obj._fitContent);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::HorizontalLayout>(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::HorizontalLayout& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gui::HorizontalLayout>(obj);
        }

		R<Core::Gui::Layout>::Deserialize<RImpl>(s, obj, true);
		s.read("_spacing", obj._spacing, 0.f);
		s.read("_fitContent", obj._fitContent, false);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gui::HorizontalLayout>(obj, s.logs());
        }
    }
}; // struct R<Core::Gui::HorizontalLayout>
// clang-format on
