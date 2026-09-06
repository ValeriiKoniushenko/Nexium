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
struct R<Core::Gui::VerticalLayout>
{
    static constexpr std::string_view Name() { return "VerticalLayout"; }
    static constexpr std::string_view FullName() { return "Core::Gui::VerticalLayout"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::optional<float>", "_spacing", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::VerticalLayout& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::VerticalLayout>(obj);
        }

		s.write(R<Core::Gui::Layout>::Serialize<RImpl>(obj).getData());
		s.write("_spacing", obj._spacing);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::VerticalLayout>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::VerticalLayout& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::VerticalLayout& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gui::VerticalLayout>(obj);
        }

		R<Core::Gui::Layout>::Deserialize<RImpl>(s, obj, true);
		s.read("_spacing", obj._spacing, decltype(obj._spacing){}, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gui::VerticalLayout>(obj, s.logs());
        }
    }

    static bool GetField(Core::Gui::VerticalLayout& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_spacing") == 0)
        {
            onFound((&obj._spacing), "std::optional<float>");
            return true;
        }

		return false;
    }
}; // struct R<Core::Gui::VerticalLayout>
// clang-format on
