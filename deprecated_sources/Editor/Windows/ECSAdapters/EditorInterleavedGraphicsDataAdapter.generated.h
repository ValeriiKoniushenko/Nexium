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
struct R<Core::ECSEditorInterleavedGraphicsDataAdapter>
{
    static constexpr std::string_view Name() { return "ECSEditorInterleavedGraphicsDataAdapter"; }
    static constexpr std::string_view FullName() { return "Core::ECSEditorInterleavedGraphicsDataAdapter"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 0; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::ECSEditorInterleavedGraphicsDataAdapter& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::ECSEditorInterleavedGraphicsDataAdapter>(obj);
        }

		s.write(R<Core::ECSEditorMimeAdapter>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::ECSEditorInterleavedGraphicsDataAdapter>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::ECSEditorInterleavedGraphicsDataAdapter& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::ECSEditorInterleavedGraphicsDataAdapter& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::ECSEditorInterleavedGraphicsDataAdapter>(obj);
        }

		R<Core::ECSEditorMimeAdapter>::Deserialize<RImpl>(s, obj, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::ECSEditorInterleavedGraphicsDataAdapter>(obj, s.logs());
        }
    }

    static bool GetField(Core::ECSEditorInterleavedGraphicsDataAdapter& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;

		return false;
    }
}; // struct R<Core::ECSEditorInterleavedGraphicsDataAdapter>
// clang-format on
