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
struct R<Core::Actor>
{
    static constexpr std::string_view Name() { return "Actor"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 0; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Actor& obj)
    {
        RResourceStream<RImpl> s;
		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write(R<Core::Transformable>::Serialize<RImpl>(obj).getData());
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Actor& obj, RResourceStream<RImpl>& s)
    {
        
		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write(R<Core::Transformable>::Serialize<RImpl>(obj).getData());
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Actor& obj)
    {
		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj);
		R<Core::Transformable>::Deserialize<RImpl>(s, obj);
    }
}; // struct R<Core::Actor>
// clang-format on
