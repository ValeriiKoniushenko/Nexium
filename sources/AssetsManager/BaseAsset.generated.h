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
struct R<Core::BaseAsset>
{
    static constexpr std::string_view Name() { return "BaseAsset"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 0; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::BaseAsset& obj)
    {
        RResourceStream<RImpl> s;
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::BaseAsset& obj, RResourceStream<RImpl>& s)
    {
        
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::BaseAsset& obj)
    {
    }
}; // struct R<Core::BaseAsset>
// clang-format on
