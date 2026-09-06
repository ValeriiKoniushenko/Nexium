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
struct R<Core::IDrawable>
{
    static constexpr std::string_view Name() { return "IDrawable"; }
    static constexpr std::string_view FullName() { return "Core::IDrawable"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "bool", "_isPostDraw", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::IDrawable& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::IDrawable>(obj);
        }

		s.write("_isPostDraw", obj._isPostDraw);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::IDrawable>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::IDrawable& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::IDrawable& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::IDrawable>(obj);
        }

		s.read("_isPostDraw", obj._isPostDraw, false, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::IDrawable>(obj, s.logs());
        }
    }

    static bool GetField(Core::IDrawable& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_isPostDraw") == 0)
        {
            onFound((&obj._isPostDraw), "bool");
            return true;
        }

		return false;
    }
}; // struct R<Core::IDrawable>
// clang-format on
