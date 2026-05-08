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
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::size_t", "_logLimit" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::LogsWindowEWC& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseFloatEWC>::Serialize<RImpl>(obj).getData());
		s.write("_logLimit", obj._logLimit);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::LogsWindowEWC& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseFloatEWC>::Serialize<RImpl>(obj).getData());
		s.write("_logLimit", obj._logLimit);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::LogsWindowEWC& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Core::BaseFloatEWC>::Deserialize<RImpl>(s, obj, true);
		s.read("_logLimit", obj._logLimit, 500);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Core::LogsWindowEWC>
// clang-format on
