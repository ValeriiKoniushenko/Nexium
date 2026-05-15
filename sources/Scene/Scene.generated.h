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
struct R<Core::Scene>
{
    static constexpr std::string_view Name() { return "Scene"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "StringAtom", "_sceneName" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Scene& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Scene>(obj);
        }

		s.write("_sceneName", obj._sceneName);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Scene>(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Scene& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Scene>(obj);
        }

		s.write("_sceneName", obj._sceneName);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Scene>(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Scene& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Scene>(obj);
        }

		s.read("_sceneName", obj._sceneName, "Default");
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Scene>(obj, s.logs());
        }
    }
}; // struct R<Core::Scene>
// clang-format on
