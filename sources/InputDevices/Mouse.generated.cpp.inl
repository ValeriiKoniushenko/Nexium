/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

namespace R
{

    namespace Core::Mouse::State
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "State"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::Mouse"; return name; }

        const std::string& ToString(::Core::Mouse::State value)
        {
            const auto& data = R::Core::Mouse::State::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static constexpr std::string empty{};
            return empty;
        }

        std::optional<::Core::Mouse::State> FromString(const std::string& value)
        {
            const auto& data = R::Core::Mouse::State::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::Mouse::State, 3>& ToArrayC()
        {
            static const std::array<::Core::Mouse::State, 3> constants = {
				::Core::Mouse::State::None,
				::Core::Mouse::State::Press,
				::Core::Mouse::State::Release
            };

            return constants;
        }

        const std::array<std::string, 3>& ToArrayN()
        {
            static const std::array<std::string, 3> names = {
				std::string("None"),
				std::string("Press"),
				std::string("Release")
            };

            return names;
        }

        const std::unordered_map<::Core::Mouse::State, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::Mouse::State, std::string> map = {
				{ ::Core::Mouse::State::None, "None" },
				{ ::Core::Mouse::State::Press, "Press" },
				{ ::Core::Mouse::State::Release, "Release" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::Mouse::State>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::Mouse::State> map = {
				{ "None", ::Core::Mouse::State::None },
				{ "Press", ::Core::Mouse::State::Press },
				{ "Release", ::Core::Mouse::State::Release }
            };

            return map;
        }

    } // namespace Core::Mouse::State


} // namespace
