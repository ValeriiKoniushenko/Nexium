/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

#include <array>
#include <optional>
#include <string>
#include <unordered_map>

template<>
struct R<Core::ShaderType>
{
    static constexpr std::string_view Name() { return "ShaderType"; }
    static constexpr std::size_t Size() { return 4; }
    static constexpr std::string_view ParentScope() { return "Core"; }

    static std::string_view ToString(::Core::ShaderType value)
    {
        const auto& data = R<Core::ShaderType>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::ShaderType> FromString(std::string_view value)
    {
        const auto& data = R<Core::ShaderType>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static consteval const std::array<::Core::ShaderType, 4>& ToArrayC()
    {
        static constexpr std::array<::Core::ShaderType, 4> constants = {
				::Core::ShaderType::None,
				::Core::ShaderType::Fragment,
				::Core::ShaderType::Vertex,
				::Core::ShaderType::Geometry
        };

        return constants;
    }

    static consteval const std::array<std::string_view, 4>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 4> names = {
				std::string_view("None"),
				std::string_view("Fragment"),
				std::string_view("Vertex"),
				std::string_view("Geometry")
        };

        return names;
    }

    static const std::unordered_map<::Core::ShaderType, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::ShaderType, std::string_view> map = {
				{ ::Core::ShaderType::None, "None" },
				{ ::Core::ShaderType::Fragment, "Fragment" },
				{ ::Core::ShaderType::Vertex, "Vertex" },
				{ ::Core::ShaderType::Geometry, "Geometry" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::ShaderType>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::ShaderType> map = {
				{ "None", ::Core::ShaderType::None },
				{ "Fragment", ::Core::ShaderType::Fragment },
				{ "Vertex", ::Core::ShaderType::Vertex },
				{ "Geometry", ::Core::ShaderType::Geometry }
        };

        return map;
    }
}; // struct R<Core::ShaderType>

