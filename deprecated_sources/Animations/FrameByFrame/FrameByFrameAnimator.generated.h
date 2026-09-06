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
struct R<Core::Animation::FrameByFrameAnimator>
{
    static constexpr std::string_view Name() { return "FrameByFrameAnimator"; }
    static constexpr std::string_view FullName() { return "Core::Animation::FrameByFrameAnimator"; }
    static constexpr std::string_view ParentScope() { return "Core::Animation"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::unordered_map<Core::StringAtom, BaseAnimation::Ptr>", "_animations", {} },
			{ "StringAtom", "_currentAnimationName", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Animation::FrameByFrameAnimator& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Animation::FrameByFrameAnimator>(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write("_animations", obj._animations);
		s.write("_currentAnimationName", obj._currentAnimationName);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Animation::FrameByFrameAnimator>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Animation::FrameByFrameAnimator& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Animation::FrameByFrameAnimator& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Animation::FrameByFrameAnimator>(obj);
        }

		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj, true);
		s.read("_animations", obj._animations, decltype(obj._animations){}, 16);
		s.read("_currentAnimationName", obj._currentAnimationName, decltype(obj._currentAnimationName){}, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Animation::FrameByFrameAnimator>(obj, s.logs());
        }
    }

    static bool GetField(Core::Animation::FrameByFrameAnimator& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_animations") == 0)
        {
            onFound((&obj._animations), "std::unordered_map<Core::StringAtom, BaseAnimation::Ptr>");
            return true;
        }
        if (std::strcmp(fieldName, "_currentAnimationName") == 0)
        {
            onFound((&obj._currentAnimationName), "StringAtom");
            return true;
        }

		return false;
    }
}; // struct R<Core::Animation::FrameByFrameAnimator>
// clang-format on
