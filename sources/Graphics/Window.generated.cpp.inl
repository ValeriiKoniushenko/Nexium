/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

namespace R
{

    namespace Core::Window::CursorMode
    {

        // =================== IMPLEMENTATIONS =====================
         const std::string& Name() { static const std::string name = "CursorMode"; return name; }
         const std::string& ParentScope() { static const std::string name = "Core::Window"; return name; }

         const std::string& ToString(::Core::Window::CursorMode value)
        {
            const auto& data = R::Core::Window::CursorMode::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static constexpr std::string empty{};
            return empty;
        }

         std::optional<::Core::Window::CursorMode> FromString(const std::string& value)
        {
            const auto& data = R::Core::Window::CursorMode::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

         const std::array<::Core::Window::CursorMode, 3>& ToArrayC()
        {
            static constexpr std::array<::Core::Window::CursorMode, 3> constants = {
				::Core::Window::CursorMode::Normal,
				::Core::Window::CursorMode::Disabled,
				::Core::Window::CursorMode::Hidden
            };

            return constants;
        }

         const std::array<std::string, 3>& ToArrayN()
        {
            static constexpr std::array<std::string, 3> names = {
				std::string("Normal"),
				std::string("Disabled"),
				std::string("Hidden")
            };

            return names;
        }

         const std::unordered_map<::Core::Window::CursorMode, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::Window::CursorMode, std::string> map = {
				{ ::Core::Window::CursorMode::Normal, "Normal" },
				{ ::Core::Window::CursorMode::Disabled, "Disabled" },
				{ ::Core::Window::CursorMode::Hidden, "Hidden" }
            };

            return map;
        }

         const std::unordered_map<std::string, ::Core::Window::CursorMode>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::Window::CursorMode> map = {
				{ "Normal", ::Core::Window::CursorMode::Normal },
				{ "Disabled", ::Core::Window::CursorMode::Disabled },
				{ "Hidden", ::Core::Window::CursorMode::Hidden }
            };

            return map;
        }

    } // namespace Core::Window::CursorMode


} // namespace
