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
struct R<Core::InputController>
{
    static constexpr std::string_view Name() { return "InputController"; }
    static constexpr std::string_view FullName() { return "Core::InputController"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::vector<Binding>", "_bindings", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::InputController& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::InputController>(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write("_bindings", obj._bindings);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::InputController>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::InputController& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::InputController& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::InputController>(obj);
        }

		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj, true);
		s.read("_bindings", obj._bindings, decltype(obj._bindings){}, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::InputController>(obj, s.logs());
        }
    }

    static bool GetField(Core::InputController& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_bindings") == 0)
        {
            onFound((&obj._bindings), "std::vector<Binding>");
            return true;
        }

		return false;
    }
}; // struct R<Core::InputController>
// clang-format on
