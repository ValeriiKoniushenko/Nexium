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
struct R<Core::Spectator>
{
    static constexpr std::string_view Name() { return "Spectator"; }
    static constexpr std::string_view FullName() { return "Core::Spectator"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "float", "speed", {} },
			{ "float", "mouseSensitivity", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Spectator& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Spectator>(obj);
        }

		s.write(R<Core::Actor>::Serialize<RImpl>(obj).getData());
		s.write("speed", obj.speed);
		s.write("mouseSensitivity", obj.mouseSensitivity);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Spectator>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Spectator& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Spectator& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Spectator>(obj);
        }

		R<Core::Actor>::Deserialize<RImpl>(s, obj, true);
		s.read("speed", obj.speed, 600.f, 16);
		s.read("mouseSensitivity", obj.mouseSensitivity, 0.5f, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Spectator>(obj, s.logs());
        }
    }

    static bool GetField(Core::Spectator& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "speed") == 0)
        {
            onFound((&obj.speed), "float");
            return true;
        }
        if (std::strcmp(fieldName, "mouseSensitivity") == 0)
        {
            onFound((&obj.mouseSensitivity), "float");
            return true;
        }

		return false;
    }
}; // struct R<Core::Spectator>
// clang-format on
