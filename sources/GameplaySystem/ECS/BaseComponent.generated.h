/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

#include <string>
#include <string_view>
#include <vector>

template<>
struct R<Core::AbstractComponent>
{
    static constexpr std::string_view Name() { return "AbstractComponent"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "bool", "_isEnabled" },
			{ "bool", "_noTick" },
		};
    }

    template<IsResourceStreamImpl RImpl>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::AbstractComponent& obj)
    {
        RResourceStream<RImpl> s;
		s.write("_isEnabled", obj._isEnabled);
		s.write("_noTick", obj._noTick);
        return s;
    }

    template<IsResourceStreamImpl RImpl>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::AbstractComponent& obj)
    {
		s.read("_isEnabled", obj._isEnabled);
		s.read("_noTick", obj._noTick);
    }
}; // struct R<Core::AbstractComponent>
