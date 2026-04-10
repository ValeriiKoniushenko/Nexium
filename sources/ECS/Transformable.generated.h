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
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 4; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "glm::vec3", "_scale" },
			{ "glm::vec3", "_origin" },
			{ "GPos3", "_position" },
			{ "glm::vec3", "_rotation" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Transformable& obj)
    {
        RResourceStream<RImpl> s;
		s.write("_scale", obj._scale);
		s.write("_origin", obj._origin);
		s.write("_position", obj._position);
		s.write("_rotation", obj._rotation);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Transformable& obj, RResourceStream<RImpl>& s)
    {
        
		s.write("_scale", obj._scale);
		s.write("_origin", obj._origin);
		s.write("_position", obj._position);
		s.write("_rotation", obj._rotation);
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Transformable& obj)
    {
		s.read("_scale", obj._scale, glm::vec3(1.f));
		s.read("_origin", obj._origin, glm::vec3(1.f));
		s.read("_position", obj._position, Core::GPos3(1.f));
		s.read("_rotation", obj._rotation, glm::vec3(1.f));
    }
}; // struct R<Core::Transformable>
// clang-format on
