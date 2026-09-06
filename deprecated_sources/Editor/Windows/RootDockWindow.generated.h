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
struct R<Core::RootDockWindowEWC>
{
    static constexpr std::string_view Name() { return "RootDockWindowEWC"; }
    static constexpr std::string_view FullName() { return "Core::RootDockWindowEWC"; }
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
    static void Serialize(const Core::RootDockWindowEWC& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::RootDockWindowEWC>(obj);
        }

		s.write(R<Core::BaseEWC>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::RootDockWindowEWC>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::RootDockWindowEWC& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::RootDockWindowEWC& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::RootDockWindowEWC>(obj);
        }

		R<Core::BaseEWC>::Deserialize<RImpl>(s, obj, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::RootDockWindowEWC>(obj, s.logs());
        }
    }

    static bool GetField(Core::RootDockWindowEWC& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;

		return false;
    }
}; // struct R<Core::RootDockWindowEWC>
// clang-format on
