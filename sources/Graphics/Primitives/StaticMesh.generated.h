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
struct R<Core::StaticMesh>
{
    static constexpr std::string_view Name() { return "StaticMesh"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "FSize3", "_size" },
			{ "glm::vec3", "_center" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::StaticMesh& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write(R<Core::Transformable>::Serialize<RImpl>(obj).getData());
		s.write(R<Core::InterleavedGraphicsData>::Serialize<RImpl>(obj).getData());
		s.write("_size", obj._size);
		s.write("_center", obj._center);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::StaticMesh& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write(R<Core::Transformable>::Serialize<RImpl>(obj).getData());
		s.write(R<Core::InterleavedGraphicsData>::Serialize<RImpl>(obj).getData());
		s.write("_size", obj._size);
		s.write("_center", obj._center);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::StaticMesh& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj, true);
		R<Core::Transformable>::Deserialize<RImpl>(s, obj, true);
		R<Core::InterleavedGraphicsData>::Deserialize<RImpl>(s, obj, true);
		s.read("_size", obj._size, 4);
		s.read("_center", obj._center, glm::vec3(0));
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Core::StaticMesh>
// clang-format on
