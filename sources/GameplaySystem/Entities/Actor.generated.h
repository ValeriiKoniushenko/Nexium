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
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Actor& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write(R<Core::Transformable>::Serialize<RImpl>(obj).getData());
		s.write(R<Core::IDrawable>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Actor& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write(R<Core::Transformable>::Serialize<RImpl>(obj).getData());
		s.write(R<Core::IDrawable>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Actor& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj, true);
		R<Core::Transformable>::Deserialize<RImpl>(s, obj, true);
		R<Core::IDrawable>::Deserialize<RImpl>(s, obj, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Core::Actor>
// clang-format on
