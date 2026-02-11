/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

namespace R
{

    namespace Core::Keyboard::KeyState
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "KeyState"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::Keyboard"; return name; }

        const std::string& ToString(::Core::Keyboard::KeyState value)
        {
            const auto& data = R::Core::Keyboard::KeyState::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static constexpr std::string empty{};
            return empty;
        }

        std::optional<::Core::Keyboard::KeyState> FromString(const std::string& value)
        {
            const auto& data = R::Core::Keyboard::KeyState::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::Keyboard::KeyState, 4>& ToArrayC()
        {
            static const std::array<::Core::Keyboard::KeyState, 4> constants = {
				::Core::Keyboard::KeyState::None,
				::Core::Keyboard::KeyState::Pressed,
				::Core::Keyboard::KeyState::Released,
				::Core::Keyboard::KeyState::Repeated
            };

            return constants;
        }

        const std::array<std::string, 4>& ToArrayN()
        {
            static const std::array<std::string, 4> names = {
				std::string("None"),
				std::string("Pressed"),
				std::string("Released"),
				std::string("Repeated")
            };

            return names;
        }

        const std::unordered_map<::Core::Keyboard::KeyState, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::Keyboard::KeyState, std::string> map = {
				{ ::Core::Keyboard::KeyState::None, "None" },
				{ ::Core::Keyboard::KeyState::Pressed, "Pressed" },
				{ ::Core::Keyboard::KeyState::Released, "Released" },
				{ ::Core::Keyboard::KeyState::Repeated, "Repeated" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::Keyboard::KeyState>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::Keyboard::KeyState> map = {
				{ "None", ::Core::Keyboard::KeyState::None },
				{ "Pressed", ::Core::Keyboard::KeyState::Pressed },
				{ "Released", ::Core::Keyboard::KeyState::Released },
				{ "Repeated", ::Core::Keyboard::KeyState::Repeated }
            };

            return map;
        }

    } // namespace Core::Keyboard::KeyState


} // namespace
