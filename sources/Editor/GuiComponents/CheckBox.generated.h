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
struct R<Core::Gui::CheckBox>
{
    static constexpr std::string_view Name() { return "CheckBox"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "glm::vec2", "_size" },
			{ "bool", "_currentValue" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::CheckBox& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::CheckBox>(obj);
        }

		s.write(R<Core::Gui::Widget>::Serialize<RImpl>(obj).getData());
		s.write("_size", obj._size);
		s.write("_currentValue", obj._currentValue);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::CheckBox>(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::CheckBox& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::CheckBox>(obj);
        }

		s.write(R<Core::Gui::Widget>::Serialize<RImpl>(obj).getData());
		s.write("_size", obj._size);
		s.write("_currentValue", obj._currentValue);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::CheckBox>(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::CheckBox& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gui::CheckBox>(obj);
        }

		R<Core::Gui::Widget>::Deserialize<RImpl>(s, obj, true);
		s.read("_size", obj._size, glm::vec2(50.f, 0.f));
		s.read("_currentValue", obj._currentValue, false);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gui::CheckBox>(obj, s.logs());
        }
    }
}; // struct R<Core::Gui::CheckBox>
// clang-format on
