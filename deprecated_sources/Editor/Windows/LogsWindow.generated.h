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
struct R<Core::LogsWindowEWC>
{
    static constexpr std::string_view Name() { return "LogsWindowEWC"; }
    static constexpr std::string_view FullName() { return "Core::LogsWindowEWC"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::size_t", "_logLimit", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::LogsWindowEWC& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::LogsWindowEWC>(obj);
        }

		s.write(R<Core::BaseFloatEWC>::Serialize<RImpl>(obj).getData());
		s.write("_logLimit", obj._logLimit);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::LogsWindowEWC>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::LogsWindowEWC& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::LogsWindowEWC& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::LogsWindowEWC>(obj);
        }

		R<Core::BaseFloatEWC>::Deserialize<RImpl>(s, obj, true);
		s.read("_logLimit", obj._logLimit, 500, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::LogsWindowEWC>(obj, s.logs());
        }
    }

    static bool GetField(Core::LogsWindowEWC& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_logLimit") == 0)
        {
            onFound((&obj._logLimit), "std::size_t");
            return true;
        }

		return false;
    }
}; // struct R<Core::LogsWindowEWC>
// clang-format on
