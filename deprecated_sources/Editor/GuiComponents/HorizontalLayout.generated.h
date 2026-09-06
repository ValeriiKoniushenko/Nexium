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
    static constexpr std::string_view FullName() { return "Core::Gui::HorizontalLayout"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "float", "_spacing", {} },
			{ "bool", "_fitContent", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
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
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::HorizontalLayout& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::HorizontalLayout& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gui::HorizontalLayout>(obj);
        }

		R<Core::Gui::Layout>::Deserialize<RImpl>(s, obj, true);
		s.read("_spacing", obj._spacing, 0.f, 16);
		s.read("_fitContent", obj._fitContent, false, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gui::HorizontalLayout>(obj, s.logs());
        }
    }

    static bool GetField(Core::Gui::HorizontalLayout& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_spacing") == 0)
        {
            onFound((&obj._spacing), "float");
            return true;
        }
        if (std::strcmp(fieldName, "_fitContent") == 0)
        {
            onFound((&obj._fitContent), "bool");
            return true;
        }

		return false;
    }
}; // struct R<Core::Gui::HorizontalLayout>
// clang-format on
