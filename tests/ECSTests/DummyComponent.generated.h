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
struct R<DummyComponent>
{
    static constexpr std::string_view Name() { return "DummyComponent"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "int", "a" },
			{ "std::string", "name" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const DummyComponent& obj)
    {
        RResourceStream<RImpl> s;
		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write("a", obj.a);
		s.write("name", obj.name);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, DummyComponent& obj)
    {
		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj);
		s.read("a", obj.a);
		s.read("name", obj.name);
    }
}; // struct R<DummyComponent>

template<>
struct R<HardConstructorComponent>
{
    static constexpr std::string_view Name() { return "HardConstructorComponent"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 0; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const HardConstructorComponent& obj)
    {
        RResourceStream<RImpl> s;
		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, HardConstructorComponent& obj)
    {
		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj);
    }
}; // struct R<HardConstructorComponent>

template<>
struct R<InitSpyComponent>
{
    static constexpr std::string_view Name() { return "InitSpyComponent"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 0; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const InitSpyComponent& obj)
    {
        RResourceStream<RImpl> s;
		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, InitSpyComponent& obj)
    {
		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj);
    }
}; // struct R<InitSpyComponent>
// clang-format on
