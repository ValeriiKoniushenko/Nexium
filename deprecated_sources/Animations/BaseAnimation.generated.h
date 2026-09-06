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
struct R<Core::Animation::BaseAnimation>
{
    static constexpr std::string_view Name() { return "BaseAnimation"; }
    static constexpr std::string_view FullName() { return "Core::Animation::BaseAnimation"; }
    static constexpr std::string_view ParentScope() { return "Core::Animation"; }
    static constexpr std::size_t GetFieldNumbers() { return 4; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "bool", "_isLooping", {} },
			{ "bool", "_isStopped", {} },
			{ "bool", "_isPaused", {} },
			{ "bool", "_isFinished", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Animation::BaseAnimation& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Animation::BaseAnimation>(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write("_isLooping", obj._isLooping);
		s.write("_isStopped", obj._isStopped);
		s.write("_isPaused", obj._isPaused);
		s.write("_isFinished", obj._isFinished);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Animation::BaseAnimation>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Animation::BaseAnimation& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Animation::BaseAnimation& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Animation::BaseAnimation>(obj);
        }

		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj, true);
		s.read("_isLooping", obj._isLooping, false, 16);
		s.read("_isStopped", obj._isStopped, true, 16);
		s.read("_isPaused", obj._isPaused, false, 16);
		s.read("_isFinished", obj._isFinished, false, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Animation::BaseAnimation>(obj, s.logs());
        }
    }

    static bool GetField(Core::Animation::BaseAnimation& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_isLooping") == 0)
        {
            onFound((&obj._isLooping), "bool");
            return true;
        }
        if (std::strcmp(fieldName, "_isStopped") == 0)
        {
            onFound((&obj._isStopped), "bool");
            return true;
        }
        if (std::strcmp(fieldName, "_isPaused") == 0)
        {
            onFound((&obj._isPaused), "bool");
            return true;
        }
        if (std::strcmp(fieldName, "_isFinished") == 0)
        {
            onFound((&obj._isFinished), "bool");
            return true;
        }

		return false;
    }
}; // struct R<Core::Animation::BaseAnimation>
// clang-format on
