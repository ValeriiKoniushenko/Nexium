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
struct R<Core::AssetsManager::NodeType>
{
    static constexpr std::string_view Name() { return "NodeType"; }
    static constexpr std::size_t Size() { return 5; }
    static constexpr std::string_view ParentScope() { return "Core::AssetsManager"; }

    static std::string_view ToString(::Core::AssetsManager::NodeType value)
    {
        const auto& data = R<Core::AssetsManager::NodeType>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::AssetsManager::NodeType> FromString(std::string_view value)
    {
        const auto& data = R<Core::AssetsManager::NodeType>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::AssetsManager::NodeType, 5>& ToArrayC()
    {
        static constexpr std::array<::Core::AssetsManager::NodeType, 5> constants = {
				::Core::AssetsManager::NodeType::Default,
				::Core::AssetsManager::NodeType::Code,
				::Core::AssetsManager::NodeType::Image,
				::Core::AssetsManager::NodeType::Folder,
				::Core::AssetsManager::NodeType::NxFile
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 5>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 5> names = {
				std::string_view("Default"),
				std::string_view("Code"),
				std::string_view("Image"),
				std::string_view("Folder"),
				std::string_view("NxFile")
        };

        return names;
    }

    static const std::unordered_map<::Core::AssetsManager::NodeType, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::AssetsManager::NodeType, std::string_view> map = {
				{ ::Core::AssetsManager::NodeType::Default, "Default" },
				{ ::Core::AssetsManager::NodeType::Code, "Code" },
				{ ::Core::AssetsManager::NodeType::Image, "Image" },
				{ ::Core::AssetsManager::NodeType::Folder, "Folder" },
				{ ::Core::AssetsManager::NodeType::NxFile, "NxFile" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::AssetsManager::NodeType>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::AssetsManager::NodeType> map = {
				{ "Default", ::Core::AssetsManager::NodeType::Default },
				{ "Code", ::Core::AssetsManager::NodeType::Code },
				{ "Image", ::Core::AssetsManager::NodeType::Image },
				{ "Folder", ::Core::AssetsManager::NodeType::Folder },
				{ "NxFile", ::Core::AssetsManager::NodeType::NxFile }
        };

        return map;
    }
}; // struct R<Core::AssetsManager::NodeType>

