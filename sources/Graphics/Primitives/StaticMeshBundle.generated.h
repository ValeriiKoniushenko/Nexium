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
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "bool", "_ignoreSelect" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::StaticMeshBundle& obj)
    {
        RResourceStream<RImpl> s;
		s.write(R<Core::Actor>::Serialize<RImpl>(obj).getData());
		s.write("_ignoreSelect", obj._ignoreSelect);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::StaticMeshBundle& obj, RResourceStream<RImpl>& s)
    {
		s.write(R<Core::Actor>::Serialize<RImpl>(obj).getData());
		s.write("_ignoreSelect", obj._ignoreSelect);
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::StaticMeshBundle& obj)
    {
		R<Core::Actor>::Deserialize<RImpl>(s, obj);
		s.read("_ignoreSelect", obj._ignoreSelect, false);
    }
}; // struct R<Core::StaticMeshBundle>
// clang-format on
