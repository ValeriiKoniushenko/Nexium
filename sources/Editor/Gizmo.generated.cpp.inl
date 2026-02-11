/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

namespace R
{

    namespace Core::Gizmo::Direction
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "Direction"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::Gizmo"; return name; }

        const std::string& ToString(::Core::Gizmo::Direction value)
        {
            const auto& data = R::Core::Gizmo::Direction::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static constexpr std::string empty{};
            return empty;
        }

        std::optional<::Core::Gizmo::Direction> FromString(const std::string& value)
        {
            const auto& data = R::Core::Gizmo::Direction::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::Gizmo::Direction, 3>& ToArrayC()
        {
            static const std::array<::Core::Gizmo::Direction, 3> constants = {
				::Core::Gizmo::Direction::X,
				::Core::Gizmo::Direction::Y,
				::Core::Gizmo::Direction::Z
            };

            return constants;
        }

        const std::array<std::string, 3>& ToArrayN()
        {
            static const std::array<std::string, 3> names = {
				std::string("X"),
				std::string("Y"),
				std::string("Z")
            };

            return names;
        }

        const std::unordered_map<::Core::Gizmo::Direction, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::Gizmo::Direction, std::string> map = {
				{ ::Core::Gizmo::Direction::X, "X" },
				{ ::Core::Gizmo::Direction::Y, "Y" },
				{ ::Core::Gizmo::Direction::Z, "Z" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::Gizmo::Direction>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::Gizmo::Direction> map = {
				{ "X", ::Core::Gizmo::Direction::X },
				{ "Y", ::Core::Gizmo::Direction::Y },
				{ "Z", ::Core::Gizmo::Direction::Z }
            };

            return map;
        }

    } // namespace Core::Gizmo::Direction


} // namespace
