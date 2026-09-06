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
struct R<Core::ECSEditorFrameByFrameAnimationAdapter>
{
    static constexpr std::string_view Name() { return "ECSEditorFrameByFrameAnimationAdapter"; }
    static constexpr std::string_view FullName() { return "Core::ECSEditorFrameByFrameAnimationAdapter"; }
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
    static void Serialize(const Core::ECSEditorFrameByFrameAnimationAdapter& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::ECSEditorFrameByFrameAnimationAdapter>(obj);
        }

		s.write(R<Core::ECSEditorMimeAdapter>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::ECSEditorFrameByFrameAnimationAdapter>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::ECSEditorFrameByFrameAnimationAdapter& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::ECSEditorFrameByFrameAnimationAdapter& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::ECSEditorFrameByFrameAnimationAdapter>(obj);
        }

		R<Core::ECSEditorMimeAdapter>::Deserialize<RImpl>(s, obj, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::ECSEditorFrameByFrameAnimationAdapter>(obj, s.logs());
        }
    }

    static bool GetField(Core::ECSEditorFrameByFrameAnimationAdapter& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;

		return false;
    }
}; // struct R<Core::ECSEditorFrameByFrameAnimationAdapter>
// clang-format on
