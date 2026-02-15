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
            static const std::string empty{};
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

    const std::string& ToString(::Core::Mouse::State value)
    {
        return Core::Mouse::State::ToString(value);
    }

    namespace Core::Mouse::Key
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "Key"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::Mouse"; return name; }

        const std::string& ToString(::Core::Mouse::Key value)
        {
            const auto& data = R::Core::Mouse::Key::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static const std::string empty{};
            return empty;
        }

        std::optional<::Core::Mouse::Key> FromString(const std::string& value)
        {
            const auto& data = R::Core::Mouse::Key::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::Mouse::Key, 13>& ToArrayC()
        {
            static const std::array<::Core::Mouse::Key, 13> constants = {
				::Core::Mouse::Key::_1,
				::Core::Mouse::Key::_2,
				::Core::Mouse::Key::_3,
				::Core::Mouse::Key::_4,
				::Core::Mouse::Key::_5,
				::Core::Mouse::Key::_6,
				::Core::Mouse::Key::_7,
				::Core::Mouse::Key::_8,
				::Core::Mouse::Key::Last,
				::Core::Mouse::Key::Left,
				::Core::Mouse::Key::Right,
				::Core::Mouse::Key::Middle,
				::Core::Mouse::Key::None
            };

            return constants;
        }

        const std::array<std::string, 13>& ToArrayN()
        {
            static const std::array<std::string, 13> names = {
				std::string("_1"),
				std::string("_2"),
				std::string("_3"),
				std::string("_4"),
				std::string("_5"),
				std::string("_6"),
				std::string("_7"),
				std::string("_8"),
				std::string("Last"),
				std::string("Left"),
				std::string("Right"),
				std::string("Middle"),
				std::string("None")
            };

            return names;
        }

        const std::unordered_map<::Core::Mouse::Key, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::Mouse::Key, std::string> map = {
				{ ::Core::Mouse::Key::_1, "_1" },
				{ ::Core::Mouse::Key::_2, "_2" },
				{ ::Core::Mouse::Key::_3, "_3" },
				{ ::Core::Mouse::Key::_4, "_4" },
				{ ::Core::Mouse::Key::_5, "_5" },
				{ ::Core::Mouse::Key::_6, "_6" },
				{ ::Core::Mouse::Key::_7, "_7" },
				{ ::Core::Mouse::Key::_8, "_8" },
				{ ::Core::Mouse::Key::Last, "Last" },
				{ ::Core::Mouse::Key::Left, "Left" },
				{ ::Core::Mouse::Key::Right, "Right" },
				{ ::Core::Mouse::Key::Middle, "Middle" },
				{ ::Core::Mouse::Key::None, "None" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::Mouse::Key>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::Mouse::Key> map = {
				{ "_1", ::Core::Mouse::Key::_1 },
				{ "_2", ::Core::Mouse::Key::_2 },
				{ "_3", ::Core::Mouse::Key::_3 },
				{ "_4", ::Core::Mouse::Key::_4 },
				{ "_5", ::Core::Mouse::Key::_5 },
				{ "_6", ::Core::Mouse::Key::_6 },
				{ "_7", ::Core::Mouse::Key::_7 },
				{ "_8", ::Core::Mouse::Key::_8 },
				{ "Last", ::Core::Mouse::Key::Last },
				{ "Left", ::Core::Mouse::Key::Left },
				{ "Right", ::Core::Mouse::Key::Right },
				{ "Middle", ::Core::Mouse::Key::Middle },
				{ "None", ::Core::Mouse::Key::None }
            };

            return map;
        }

    } // namespace Core::Mouse::Key

    const std::string& ToString(::Core::Mouse::Key value)
    {
        return Core::Mouse::Key::ToString(value);
    }

    namespace Core::Mouse::Mod
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "Mod"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::Mouse"; return name; }

        const std::string& ToString(::Core::Mouse::Mod value)
        {
            const auto& data = R::Core::Mouse::Mod::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static const std::string empty{};
            return empty;
        }

        std::optional<::Core::Mouse::Mod> FromString(const std::string& value)
        {
            const auto& data = R::Core::Mouse::Mod::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::Mouse::Mod, 7>& ToArrayC()
        {
            static const std::array<::Core::Mouse::Mod, 7> constants = {
				::Core::Mouse::Mod::None,
				::Core::Mouse::Mod::Shift,
				::Core::Mouse::Mod::Control,
				::Core::Mouse::Mod::Alt,
				::Core::Mouse::Mod::Super,
				::Core::Mouse::Mod::CapsLock,
				::Core::Mouse::Mod::NumLock
            };

            return constants;
        }

        const std::array<std::string, 7>& ToArrayN()
        {
            static const std::array<std::string, 7> names = {
				std::string("None"),
				std::string("Shift"),
				std::string("Control"),
				std::string("Alt"),
				std::string("Super"),
				std::string("CapsLock"),
				std::string("NumLock")
            };

            return names;
        }

        const std::unordered_map<::Core::Mouse::Mod, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::Mouse::Mod, std::string> map = {
				{ ::Core::Mouse::Mod::None, "None" },
				{ ::Core::Mouse::Mod::Shift, "Shift" },
				{ ::Core::Mouse::Mod::Control, "Control" },
				{ ::Core::Mouse::Mod::Alt, "Alt" },
				{ ::Core::Mouse::Mod::Super, "Super" },
				{ ::Core::Mouse::Mod::CapsLock, "CapsLock" },
				{ ::Core::Mouse::Mod::NumLock, "NumLock" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::Mouse::Mod>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::Mouse::Mod> map = {
				{ "None", ::Core::Mouse::Mod::None },
				{ "Shift", ::Core::Mouse::Mod::Shift },
				{ "Control", ::Core::Mouse::Mod::Control },
				{ "Alt", ::Core::Mouse::Mod::Alt },
				{ "Super", ::Core::Mouse::Mod::Super },
				{ "CapsLock", ::Core::Mouse::Mod::CapsLock },
				{ "NumLock", ::Core::Mouse::Mod::NumLock }
            };

            return map;
        }

    } // namespace Core::Mouse::Mod

    const std::string& ToString(::Core::Mouse::Mod value)
    {
        return Core::Mouse::Mod::ToString(value);
    }


} // namespace
