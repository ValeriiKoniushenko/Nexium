/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

namespace R
{

    namespace Core::AssetsManager::NodeType
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "NodeType"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::AssetsManager"; return name; }

        const std::string& ToString(::Core::AssetsManager::NodeType value)
        {
            const auto& data = R::Core::AssetsManager::NodeType::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static constexpr std::string empty{};
            return empty;
        }

        std::optional<::Core::AssetsManager::NodeType> FromString(const std::string& value)
        {
            const auto& data = R::Core::AssetsManager::NodeType::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::AssetsManager::NodeType, 5>& ToArrayC()
        {
            static const std::array<::Core::AssetsManager::NodeType, 5> constants = {
				::Core::AssetsManager::NodeType::Default,
				::Core::AssetsManager::NodeType::Code,
				::Core::AssetsManager::NodeType::Image,
				::Core::AssetsManager::NodeType::Folder,
				::Core::AssetsManager::NodeType::NxFile
            };

            return constants;
        }

        const std::array<std::string, 5>& ToArrayN()
        {
            static const std::array<std::string, 5> names = {
				std::string("Default"),
				std::string("Code"),
				std::string("Image"),
				std::string("Folder"),
				std::string("NxFile")
            };

            return names;
        }

        const std::unordered_map<::Core::AssetsManager::NodeType, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::AssetsManager::NodeType, std::string> map = {
				{ ::Core::AssetsManager::NodeType::Default, "Default" },
				{ ::Core::AssetsManager::NodeType::Code, "Code" },
				{ ::Core::AssetsManager::NodeType::Image, "Image" },
				{ ::Core::AssetsManager::NodeType::Folder, "Folder" },
				{ ::Core::AssetsManager::NodeType::NxFile, "NxFile" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::AssetsManager::NodeType>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::AssetsManager::NodeType> map = {
				{ "Default", ::Core::AssetsManager::NodeType::Default },
				{ "Code", ::Core::AssetsManager::NodeType::Code },
				{ "Image", ::Core::AssetsManager::NodeType::Image },
				{ "Folder", ::Core::AssetsManager::NodeType::Folder },
				{ "NxFile", ::Core::AssetsManager::NodeType::NxFile }
            };

            return map;
        }

    } // namespace Core::AssetsManager::NodeType


} // namespace
