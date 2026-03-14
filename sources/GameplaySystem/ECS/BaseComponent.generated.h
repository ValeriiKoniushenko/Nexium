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
    static constexpr std::size_t GetFieldNumbers() { return 0; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
		};
    }

    template<IsResourceStreamImpl RImpl>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::AbstractComponent& obj)
    {
        RResourceStream<RImpl> s;
        return s;
    }

    template<IsResourceStreamImpl RImpl>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::AbstractComponent& obj)
    {
    }
}; // struct R<Core::AbstractComponent>
