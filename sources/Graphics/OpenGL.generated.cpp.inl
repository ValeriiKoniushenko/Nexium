/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

namespace R
{

    namespace Core::ViewportMode
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "ViewportMode"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core"; return name; }

        const std::string& ToString(::Core::ViewportMode value)
        {
            const auto& data = R::Core::ViewportMode::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static const std::string empty{};
            return empty;
        }

        std::optional<::Core::ViewportMode> FromString(const std::string& value)
        {
            const auto& data = R::Core::ViewportMode::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::ViewportMode, 2>& ToArrayC()
        {
            static const std::array<::Core::ViewportMode, 2> constants = {
				::Core::ViewportMode::Default,
				::Core::ViewportMode::ZoomIn
            };

            return constants;
        }

        const std::array<std::string, 2>& ToArrayN()
        {
            static const std::array<std::string, 2> names = {
				std::string("Default"),
				std::string("ZoomIn")
            };

            return names;
        }

        const std::unordered_map<::Core::ViewportMode, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::ViewportMode, std::string> map = {
				{ ::Core::ViewportMode::Default, "Default" },
				{ ::Core::ViewportMode::ZoomIn, "ZoomIn" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::ViewportMode>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::ViewportMode> map = {
				{ "Default", ::Core::ViewportMode::Default },
				{ "ZoomIn", ::Core::ViewportMode::ZoomIn }
            };

            return map;
        }

    } // namespace Core::ViewportMode

    const std::string& ToString(::Core::ViewportMode value)
    {
        return Core::ViewportMode::ToString(value);
    }


} // namespace
