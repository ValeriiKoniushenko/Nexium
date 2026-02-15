/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

namespace R
{

    namespace Core::ShaderType
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "ShaderType"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core"; return name; }

        const std::string& ToString(::Core::ShaderType value)
        {
            const auto& data = R::Core::ShaderType::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static const std::string empty{};
            return empty;
        }

        std::optional<::Core::ShaderType> FromString(const std::string& value)
        {
            const auto& data = R::Core::ShaderType::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::ShaderType, 4>& ToArrayC()
        {
            static const std::array<::Core::ShaderType, 4> constants = {
				::Core::ShaderType::None,
				::Core::ShaderType::Fragment,
				::Core::ShaderType::Vertex,
				::Core::ShaderType::Geometry
            };

            return constants;
        }

        const std::array<std::string, 4>& ToArrayN()
        {
            static const std::array<std::string, 4> names = {
				std::string("None"),
				std::string("Fragment"),
				std::string("Vertex"),
				std::string("Geometry")
            };

            return names;
        }

        const std::unordered_map<::Core::ShaderType, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::ShaderType, std::string> map = {
				{ ::Core::ShaderType::None, "None" },
				{ ::Core::ShaderType::Fragment, "Fragment" },
				{ ::Core::ShaderType::Vertex, "Vertex" },
				{ ::Core::ShaderType::Geometry, "Geometry" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::ShaderType>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::ShaderType> map = {
				{ "None", ::Core::ShaderType::None },
				{ "Fragment", ::Core::ShaderType::Fragment },
				{ "Vertex", ::Core::ShaderType::Vertex },
				{ "Geometry", ::Core::ShaderType::Geometry }
            };

            return map;
        }

    } // namespace Core::ShaderType

    const std::string& ToString(::Core::ShaderType value)
    {
        return Core::ShaderType::ToString(value);
    }


} // namespace
