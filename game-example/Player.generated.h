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
struct R<Player>
{
    static constexpr std::string_view Name() { return "Player"; }
    static constexpr std::string_view FullName() { return "Player"; }
    static constexpr std::string_view ParentScope() { return ""; }
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
    static void Serialize(const Player& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Player>(obj);
        }

		s.write(R<Core::SceneObj::RectangleAnimated>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize<Player>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Player& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Player& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Player>(obj);
        }

		R<Core::SceneObj::RectangleAnimated>::Deserialize<RImpl>(s, obj, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Player>(obj, s.logs());
        }
    }

    static bool GetField(Player& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;

		return false;
    }
}; // struct R<Player>
// clang-format on
