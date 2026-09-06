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
struct R<Core::Transformable>
{
    static constexpr std::string_view Name() { return "Transformable"; }
    static constexpr std::string_view FullName() { return "Core::Transformable"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 4; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "glm::vec3", "_scale", {} },
			{ "glm::vec3", "_origin", {} },
			{ "GPos3", "_position", {} },
			{ "glm::vec3", "_rotation", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Transformable& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Transformable>(obj);
        }

		s.write("_scale", obj._scale);
		s.write("_origin", obj._origin);
		s.write("_position", obj._position);
		s.write("_rotation", obj._rotation);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Transformable>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Transformable& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Transformable& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Transformable>(obj);
        }

		s.read("_scale", obj._scale, glm::vec3(1.f), 16);
		s.read("_origin", obj._origin, glm::vec3(0.f), 16);
		s.read("_position", obj._position, Core::GPos3(0.f), 16);
		s.read("_rotation", obj._rotation, glm::vec3(0.f), 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Transformable>(obj, s.logs());
        }
    }

    static bool GetField(Core::Transformable& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_scale") == 0)
        {
            onFound((&obj._scale), "glm::vec3");
            return true;
        }
        if (std::strcmp(fieldName, "_origin") == 0)
        {
            onFound((&obj._origin), "glm::vec3");
            return true;
        }
        if (std::strcmp(fieldName, "_position") == 0)
        {
            onFound((&obj._position), "GPos3");
            return true;
        }
        if (std::strcmp(fieldName, "_rotation") == 0)
        {
            onFound((&obj._rotation), "glm::vec3");
            return true;
        }

		return false;
    }
}; // struct R<Core::Transformable>
// clang-format on
