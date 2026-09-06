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
    static constexpr std::string_view FullName() { return "Core::StaticMesh"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "FSize3", "_size", {} },
			{ "glm::vec3", "_center", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::StaticMesh& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::StaticMesh>(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write(R<Core::Transformable>::Serialize<RImpl>(obj).getData());
		s.write(R<Core::InterleavedGraphicsData>::Serialize<RImpl>(obj).getData());
		s.write("_size", obj._size);
		s.write("_center", obj._center);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::StaticMesh>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::StaticMesh& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::StaticMesh& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::StaticMesh>(obj);
        }

		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj, true);
		R<Core::Transformable>::Deserialize<RImpl>(s, obj, true);
		R<Core::InterleavedGraphicsData>::Deserialize<RImpl>(s, obj, true);
		s.read("_size", obj._size, decltype(obj._size){}, 16);
		s.read("_center", obj._center, glm::vec3(0), 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::StaticMesh>(obj, s.logs());
        }
    }

    static bool GetField(Core::StaticMesh& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_size") == 0)
        {
            onFound((&obj._size), "FSize3");
            return true;
        }
        if (std::strcmp(fieldName, "_center") == 0)
        {
            onFound((&obj._center), "glm::vec3");
            return true;
        }

		return false;
    }
}; // struct R<Core::StaticMesh>
// clang-format on
