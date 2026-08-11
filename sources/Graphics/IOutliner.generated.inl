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
struct R<Core::IOutliner>
{
    static constexpr std::string_view Name() { return "IOutliner"; }
    static constexpr std::string_view FullName() { return "Core::IOutliner"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "bool", "_isDrawOutline" },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::IOutliner& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::IOutliner>(obj);
        }

		s.write("_isDrawOutline", obj._isDrawOutline);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::IOutliner>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::IOutliner& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::IOutliner& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::IOutliner>(obj);
        }

		s.read("_isDrawOutline", obj._isDrawOutline, false);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::IOutliner>(obj, s.logs());
        }
    }
}; // struct R<Core::IOutliner>
// clang-format on
