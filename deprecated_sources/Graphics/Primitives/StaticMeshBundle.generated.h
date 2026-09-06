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
struct R<Core::StaticMeshBundle>
{
    static constexpr std::string_view Name() { return "StaticMeshBundle"; }
    static constexpr std::string_view FullName() { return "Core::StaticMeshBundle"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "bool", "_ignoreSelect", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::StaticMeshBundle& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::StaticMeshBundle>(obj);
        }

		s.write(R<Core::Actor>::Serialize<RImpl>(obj).getData());
		s.write("_ignoreSelect", obj._ignoreSelect);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::StaticMeshBundle>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::StaticMeshBundle& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::StaticMeshBundle& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::StaticMeshBundle>(obj);
        }

		R<Core::Actor>::Deserialize<RImpl>(s, obj, true);
		s.read("_ignoreSelect", obj._ignoreSelect, false, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::StaticMeshBundle>(obj, s.logs());
        }
    }

    static bool GetField(Core::StaticMeshBundle& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_ignoreSelect") == 0)
        {
            onFound((&obj._ignoreSelect), "bool");
            return true;
        }

		return false;
    }
}; // struct R<Core::StaticMeshBundle>
// clang-format on
