/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

namespace R
{

    namespace Core::GraphicsComponentData::Modifier
    {

        // =================== IMPLEMENTATIONS =====================
         const std::string& Name() { static const std::string name = "Modifier"; return name; }
         const std::string& ParentScope() { static const std::string name = "Core::GraphicsComponentData"; return name; }

         const std::string& ToString(::Core::GraphicsComponentData::Modifier value)
        {
            const auto& data = R::Core::GraphicsComponentData::Modifier::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static constexpr std::string empty{};
            return empty;
        }

         std::optional<::Core::GraphicsComponentData::Modifier> FromString(const std::string& value)
        {
            const auto& data = R::Core::GraphicsComponentData::Modifier::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

         const std::array<::Core::GraphicsComponentData::Modifier, 3>& ToArrayC()
        {
            static constexpr std::array<::Core::GraphicsComponentData::Modifier, 3> constants = {
				::Core::GraphicsComponentData::Modifier::None,
				::Core::GraphicsComponentData::Modifier::Enable,
				::Core::GraphicsComponentData::Modifier::Disable
            };

            return constants;
        }

         const std::array<std::string, 3>& ToArrayN()
        {
            static constexpr std::array<std::string, 3> names = {
				std::string("None"),
				std::string("Enable"),
				std::string("Disable")
            };

            return names;
        }

         const std::unordered_map<::Core::GraphicsComponentData::Modifier, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::GraphicsComponentData::Modifier, std::string> map = {
				{ ::Core::GraphicsComponentData::Modifier::None, "None" },
				{ ::Core::GraphicsComponentData::Modifier::Enable, "Enable" },
				{ ::Core::GraphicsComponentData::Modifier::Disable, "Disable" }
            };

            return map;
        }

         const std::unordered_map<std::string, ::Core::GraphicsComponentData::Modifier>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::GraphicsComponentData::Modifier> map = {
				{ "None", ::Core::GraphicsComponentData::Modifier::None },
				{ "Enable", ::Core::GraphicsComponentData::Modifier::Enable },
				{ "Disable", ::Core::GraphicsComponentData::Modifier::Disable }
            };

            return map;
        }

    } // namespace Core::GraphicsComponentData::Modifier


} // namespace
