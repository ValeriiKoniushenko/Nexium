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

    const std::string& ToString(::Core::Keyboard::KeyState value)
    {
        return Core::Keyboard::KeyState::ToString(value);
    }

    namespace Core::Keyboard::Key
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "Key"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::Keyboard"; return name; }

        const std::string& ToString(::Core::Keyboard::Key value)
        {
            const auto& data = R::Core::Keyboard::Key::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static constexpr std::string empty{};
            return empty;
        }

        std::optional<::Core::Keyboard::Key> FromString(const std::string& value)
        {
            const auto& data = R::Core::Keyboard::Key::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::Keyboard::Key, 122>& ToArrayC()
        {
            static const std::array<::Core::Keyboard::Key, 122> constants = {
				::Core::Keyboard::Key::None,
				::Core::Keyboard::Key::Space,
				::Core::Keyboard::Key::Apostrophe,
				::Core::Keyboard::Key::Comma,
				::Core::Keyboard::Key::Minus,
				::Core::Keyboard::Key::Period,
				::Core::Keyboard::Key::Slash,
				::Core::Keyboard::Key::_0,
				::Core::Keyboard::Key::_1,
				::Core::Keyboard::Key::_2,
				::Core::Keyboard::Key::_3,
				::Core::Keyboard::Key::_4,
				::Core::Keyboard::Key::_5,
				::Core::Keyboard::Key::_6,
				::Core::Keyboard::Key::_7,
				::Core::Keyboard::Key::_8,
				::Core::Keyboard::Key::_9,
				::Core::Keyboard::Key::Semicolon,
				::Core::Keyboard::Key::Equal,
				::Core::Keyboard::Key::A,
				::Core::Keyboard::Key::B,
				::Core::Keyboard::Key::C,
				::Core::Keyboard::Key::D,
				::Core::Keyboard::Key::E,
				::Core::Keyboard::Key::F,
				::Core::Keyboard::Key::G,
				::Core::Keyboard::Key::H,
				::Core::Keyboard::Key::I,
				::Core::Keyboard::Key::J,
				::Core::Keyboard::Key::K,
				::Core::Keyboard::Key::L,
				::Core::Keyboard::Key::M,
				::Core::Keyboard::Key::N,
				::Core::Keyboard::Key::O,
				::Core::Keyboard::Key::P,
				::Core::Keyboard::Key::Q,
				::Core::Keyboard::Key::R,
				::Core::Keyboard::Key::S,
				::Core::Keyboard::Key::T,
				::Core::Keyboard::Key::U,
				::Core::Keyboard::Key::V,
				::Core::Keyboard::Key::W,
				::Core::Keyboard::Key::X,
				::Core::Keyboard::Key::Y,
				::Core::Keyboard::Key::Z,
				::Core::Keyboard::Key::Left_Bracket,
				::Core::Keyboard::Key::Backslash,
				::Core::Keyboard::Key::Right_Bracket,
				::Core::Keyboard::Key::Grave_Accent,
				::Core::Keyboard::Key::World_1,
				::Core::Keyboard::Key::World_2,
				::Core::Keyboard::Key::Escape,
				::Core::Keyboard::Key::Enter,
				::Core::Keyboard::Key::Tab,
				::Core::Keyboard::Key::Backspace,
				::Core::Keyboard::Key::Insert,
				::Core::Keyboard::Key::Delete,
				::Core::Keyboard::Key::Right,
				::Core::Keyboard::Key::Left,
				::Core::Keyboard::Key::Down,
				::Core::Keyboard::Key::Up,
				::Core::Keyboard::Key::Page_Up,
				::Core::Keyboard::Key::Page_Down,
				::Core::Keyboard::Key::Home,
				::Core::Keyboard::Key::End,
				::Core::Keyboard::Key::Caps_Lock,
				::Core::Keyboard::Key::Scroll_Lock,
				::Core::Keyboard::Key::Num_Lock,
				::Core::Keyboard::Key::Print_Screen,
				::Core::Keyboard::Key::Pause,
				::Core::Keyboard::Key::F1,
				::Core::Keyboard::Key::F2,
				::Core::Keyboard::Key::F3,
				::Core::Keyboard::Key::F4,
				::Core::Keyboard::Key::F5,
				::Core::Keyboard::Key::F6,
				::Core::Keyboard::Key::F7,
				::Core::Keyboard::Key::F8,
				::Core::Keyboard::Key::F9,
				::Core::Keyboard::Key::F10,
				::Core::Keyboard::Key::F11,
				::Core::Keyboard::Key::F12,
				::Core::Keyboard::Key::F13,
				::Core::Keyboard::Key::F14,
				::Core::Keyboard::Key::F15,
				::Core::Keyboard::Key::F16,
				::Core::Keyboard::Key::F17,
				::Core::Keyboard::Key::F18,
				::Core::Keyboard::Key::F19,
				::Core::Keyboard::Key::F20,
				::Core::Keyboard::Key::F21,
				::Core::Keyboard::Key::F22,
				::Core::Keyboard::Key::F23,
				::Core::Keyboard::Key::F24,
				::Core::Keyboard::Key::F25,
				::Core::Keyboard::Key::Kp_0,
				::Core::Keyboard::Key::Kp_1,
				::Core::Keyboard::Key::Kp_2,
				::Core::Keyboard::Key::Kp_3,
				::Core::Keyboard::Key::Kp_4,
				::Core::Keyboard::Key::Kp_5,
				::Core::Keyboard::Key::Kp_6,
				::Core::Keyboard::Key::Kp_7,
				::Core::Keyboard::Key::Kp_8,
				::Core::Keyboard::Key::Kp_9,
				::Core::Keyboard::Key::Kp_Decimal,
				::Core::Keyboard::Key::Kp_Divide,
				::Core::Keyboard::Key::Kp_Multiply,
				::Core::Keyboard::Key::Kp_Subtract,
				::Core::Keyboard::Key::Kp_Add,
				::Core::Keyboard::Key::Kp_Enter,
				::Core::Keyboard::Key::Kp_Equal,
				::Core::Keyboard::Key::Left_Shift,
				::Core::Keyboard::Key::Left_Control,
				::Core::Keyboard::Key::Left_Alt,
				::Core::Keyboard::Key::Left_Super,
				::Core::Keyboard::Key::Right_Shift,
				::Core::Keyboard::Key::Right_Control,
				::Core::Keyboard::Key::Right_Alt,
				::Core::Keyboard::Key::Right_Super,
				::Core::Keyboard::Key::Menu,
				::Core::Keyboard::Key::Last
            };

            return constants;
        }

        const std::array<std::string, 122>& ToArrayN()
        {
            static const std::array<std::string, 122> names = {
				std::string("None"),
				std::string("Space"),
				std::string("Apostrophe"),
				std::string("Comma"),
				std::string("Minus"),
				std::string("Period"),
				std::string("Slash"),
				std::string("_0"),
				std::string("_1"),
				std::string("_2"),
				std::string("_3"),
				std::string("_4"),
				std::string("_5"),
				std::string("_6"),
				std::string("_7"),
				std::string("_8"),
				std::string("_9"),
				std::string("Semicolon"),
				std::string("Equal"),
				std::string("A"),
				std::string("B"),
				std::string("C"),
				std::string("D"),
				std::string("E"),
				std::string("F"),
				std::string("G"),
				std::string("H"),
				std::string("I"),
				std::string("J"),
				std::string("K"),
				std::string("L"),
				std::string("M"),
				std::string("N"),
				std::string("O"),
				std::string("P"),
				std::string("Q"),
				std::string("R"),
				std::string("S"),
				std::string("T"),
				std::string("U"),
				std::string("V"),
				std::string("W"),
				std::string("X"),
				std::string("Y"),
				std::string("Z"),
				std::string("Left_Bracket"),
				std::string("Backslash"),
				std::string("Right_Bracket"),
				std::string("Grave_Accent"),
				std::string("World_1"),
				std::string("World_2"),
				std::string("Escape"),
				std::string("Enter"),
				std::string("Tab"),
				std::string("Backspace"),
				std::string("Insert"),
				std::string("Delete"),
				std::string("Right"),
				std::string("Left"),
				std::string("Down"),
				std::string("Up"),
				std::string("Page_Up"),
				std::string("Page_Down"),
				std::string("Home"),
				std::string("End"),
				std::string("Caps_Lock"),
				std::string("Scroll_Lock"),
				std::string("Num_Lock"),
				std::string("Print_Screen"),
				std::string("Pause"),
				std::string("F1"),
				std::string("F2"),
				std::string("F3"),
				std::string("F4"),
				std::string("F5"),
				std::string("F6"),
				std::string("F7"),
				std::string("F8"),
				std::string("F9"),
				std::string("F10"),
				std::string("F11"),
				std::string("F12"),
				std::string("F13"),
				std::string("F14"),
				std::string("F15"),
				std::string("F16"),
				std::string("F17"),
				std::string("F18"),
				std::string("F19"),
				std::string("F20"),
				std::string("F21"),
				std::string("F22"),
				std::string("F23"),
				std::string("F24"),
				std::string("F25"),
				std::string("Kp_0"),
				std::string("Kp_1"),
				std::string("Kp_2"),
				std::string("Kp_3"),
				std::string("Kp_4"),
				std::string("Kp_5"),
				std::string("Kp_6"),
				std::string("Kp_7"),
				std::string("Kp_8"),
				std::string("Kp_9"),
				std::string("Kp_Decimal"),
				std::string("Kp_Divide"),
				std::string("Kp_Multiply"),
				std::string("Kp_Subtract"),
				std::string("Kp_Add"),
				std::string("Kp_Enter"),
				std::string("Kp_Equal"),
				std::string("Left_Shift"),
				std::string("Left_Control"),
				std::string("Left_Alt"),
				std::string("Left_Super"),
				std::string("Right_Shift"),
				std::string("Right_Control"),
				std::string("Right_Alt"),
				std::string("Right_Super"),
				std::string("Menu"),
				std::string("Last")
            };

            return names;
        }

        const std::unordered_map<::Core::Keyboard::Key, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::Keyboard::Key, std::string> map = {
				{ ::Core::Keyboard::Key::None, "None" },
				{ ::Core::Keyboard::Key::Space, "Space" },
				{ ::Core::Keyboard::Key::Apostrophe, "Apostrophe" },
				{ ::Core::Keyboard::Key::Comma, "Comma" },
				{ ::Core::Keyboard::Key::Minus, "Minus" },
				{ ::Core::Keyboard::Key::Period, "Period" },
				{ ::Core::Keyboard::Key::Slash, "Slash" },
				{ ::Core::Keyboard::Key::_0, "_0" },
				{ ::Core::Keyboard::Key::_1, "_1" },
				{ ::Core::Keyboard::Key::_2, "_2" },
				{ ::Core::Keyboard::Key::_3, "_3" },
				{ ::Core::Keyboard::Key::_4, "_4" },
				{ ::Core::Keyboard::Key::_5, "_5" },
				{ ::Core::Keyboard::Key::_6, "_6" },
				{ ::Core::Keyboard::Key::_7, "_7" },
				{ ::Core::Keyboard::Key::_8, "_8" },
				{ ::Core::Keyboard::Key::_9, "_9" },
				{ ::Core::Keyboard::Key::Semicolon, "Semicolon" },
				{ ::Core::Keyboard::Key::Equal, "Equal" },
				{ ::Core::Keyboard::Key::A, "A" },
				{ ::Core::Keyboard::Key::B, "B" },
				{ ::Core::Keyboard::Key::C, "C" },
				{ ::Core::Keyboard::Key::D, "D" },
				{ ::Core::Keyboard::Key::E, "E" },
				{ ::Core::Keyboard::Key::F, "F" },
				{ ::Core::Keyboard::Key::G, "G" },
				{ ::Core::Keyboard::Key::H, "H" },
				{ ::Core::Keyboard::Key::I, "I" },
				{ ::Core::Keyboard::Key::J, "J" },
				{ ::Core::Keyboard::Key::K, "K" },
				{ ::Core::Keyboard::Key::L, "L" },
				{ ::Core::Keyboard::Key::M, "M" },
				{ ::Core::Keyboard::Key::N, "N" },
				{ ::Core::Keyboard::Key::O, "O" },
				{ ::Core::Keyboard::Key::P, "P" },
				{ ::Core::Keyboard::Key::Q, "Q" },
				{ ::Core::Keyboard::Key::R, "R" },
				{ ::Core::Keyboard::Key::S, "S" },
				{ ::Core::Keyboard::Key::T, "T" },
				{ ::Core::Keyboard::Key::U, "U" },
				{ ::Core::Keyboard::Key::V, "V" },
				{ ::Core::Keyboard::Key::W, "W" },
				{ ::Core::Keyboard::Key::X, "X" },
				{ ::Core::Keyboard::Key::Y, "Y" },
				{ ::Core::Keyboard::Key::Z, "Z" },
				{ ::Core::Keyboard::Key::Left_Bracket, "Left_Bracket" },
				{ ::Core::Keyboard::Key::Backslash, "Backslash" },
				{ ::Core::Keyboard::Key::Right_Bracket, "Right_Bracket" },
				{ ::Core::Keyboard::Key::Grave_Accent, "Grave_Accent" },
				{ ::Core::Keyboard::Key::World_1, "World_1" },
				{ ::Core::Keyboard::Key::World_2, "World_2" },
				{ ::Core::Keyboard::Key::Escape, "Escape" },
				{ ::Core::Keyboard::Key::Enter, "Enter" },
				{ ::Core::Keyboard::Key::Tab, "Tab" },
				{ ::Core::Keyboard::Key::Backspace, "Backspace" },
				{ ::Core::Keyboard::Key::Insert, "Insert" },
				{ ::Core::Keyboard::Key::Delete, "Delete" },
				{ ::Core::Keyboard::Key::Right, "Right" },
				{ ::Core::Keyboard::Key::Left, "Left" },
				{ ::Core::Keyboard::Key::Down, "Down" },
				{ ::Core::Keyboard::Key::Up, "Up" },
				{ ::Core::Keyboard::Key::Page_Up, "Page_Up" },
				{ ::Core::Keyboard::Key::Page_Down, "Page_Down" },
				{ ::Core::Keyboard::Key::Home, "Home" },
				{ ::Core::Keyboard::Key::End, "End" },
				{ ::Core::Keyboard::Key::Caps_Lock, "Caps_Lock" },
				{ ::Core::Keyboard::Key::Scroll_Lock, "Scroll_Lock" },
				{ ::Core::Keyboard::Key::Num_Lock, "Num_Lock" },
				{ ::Core::Keyboard::Key::Print_Screen, "Print_Screen" },
				{ ::Core::Keyboard::Key::Pause, "Pause" },
				{ ::Core::Keyboard::Key::F1, "F1" },
				{ ::Core::Keyboard::Key::F2, "F2" },
				{ ::Core::Keyboard::Key::F3, "F3" },
				{ ::Core::Keyboard::Key::F4, "F4" },
				{ ::Core::Keyboard::Key::F5, "F5" },
				{ ::Core::Keyboard::Key::F6, "F6" },
				{ ::Core::Keyboard::Key::F7, "F7" },
				{ ::Core::Keyboard::Key::F8, "F8" },
				{ ::Core::Keyboard::Key::F9, "F9" },
				{ ::Core::Keyboard::Key::F10, "F10" },
				{ ::Core::Keyboard::Key::F11, "F11" },
				{ ::Core::Keyboard::Key::F12, "F12" },
				{ ::Core::Keyboard::Key::F13, "F13" },
				{ ::Core::Keyboard::Key::F14, "F14" },
				{ ::Core::Keyboard::Key::F15, "F15" },
				{ ::Core::Keyboard::Key::F16, "F16" },
				{ ::Core::Keyboard::Key::F17, "F17" },
				{ ::Core::Keyboard::Key::F18, "F18" },
				{ ::Core::Keyboard::Key::F19, "F19" },
				{ ::Core::Keyboard::Key::F20, "F20" },
				{ ::Core::Keyboard::Key::F21, "F21" },
				{ ::Core::Keyboard::Key::F22, "F22" },
				{ ::Core::Keyboard::Key::F23, "F23" },
				{ ::Core::Keyboard::Key::F24, "F24" },
				{ ::Core::Keyboard::Key::F25, "F25" },
				{ ::Core::Keyboard::Key::Kp_0, "Kp_0" },
				{ ::Core::Keyboard::Key::Kp_1, "Kp_1" },
				{ ::Core::Keyboard::Key::Kp_2, "Kp_2" },
				{ ::Core::Keyboard::Key::Kp_3, "Kp_3" },
				{ ::Core::Keyboard::Key::Kp_4, "Kp_4" },
				{ ::Core::Keyboard::Key::Kp_5, "Kp_5" },
				{ ::Core::Keyboard::Key::Kp_6, "Kp_6" },
				{ ::Core::Keyboard::Key::Kp_7, "Kp_7" },
				{ ::Core::Keyboard::Key::Kp_8, "Kp_8" },
				{ ::Core::Keyboard::Key::Kp_9, "Kp_9" },
				{ ::Core::Keyboard::Key::Kp_Decimal, "Kp_Decimal" },
				{ ::Core::Keyboard::Key::Kp_Divide, "Kp_Divide" },
				{ ::Core::Keyboard::Key::Kp_Multiply, "Kp_Multiply" },
				{ ::Core::Keyboard::Key::Kp_Subtract, "Kp_Subtract" },
				{ ::Core::Keyboard::Key::Kp_Add, "Kp_Add" },
				{ ::Core::Keyboard::Key::Kp_Enter, "Kp_Enter" },
				{ ::Core::Keyboard::Key::Kp_Equal, "Kp_Equal" },
				{ ::Core::Keyboard::Key::Left_Shift, "Left_Shift" },
				{ ::Core::Keyboard::Key::Left_Control, "Left_Control" },
				{ ::Core::Keyboard::Key::Left_Alt, "Left_Alt" },
				{ ::Core::Keyboard::Key::Left_Super, "Left_Super" },
				{ ::Core::Keyboard::Key::Right_Shift, "Right_Shift" },
				{ ::Core::Keyboard::Key::Right_Control, "Right_Control" },
				{ ::Core::Keyboard::Key::Right_Alt, "Right_Alt" },
				{ ::Core::Keyboard::Key::Right_Super, "Right_Super" },
				{ ::Core::Keyboard::Key::Menu, "Menu" },
				{ ::Core::Keyboard::Key::Last, "Last" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::Keyboard::Key>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::Keyboard::Key> map = {
				{ "None", ::Core::Keyboard::Key::None },
				{ "Space", ::Core::Keyboard::Key::Space },
				{ "Apostrophe", ::Core::Keyboard::Key::Apostrophe },
				{ "Comma", ::Core::Keyboard::Key::Comma },
				{ "Minus", ::Core::Keyboard::Key::Minus },
				{ "Period", ::Core::Keyboard::Key::Period },
				{ "Slash", ::Core::Keyboard::Key::Slash },
				{ "_0", ::Core::Keyboard::Key::_0 },
				{ "_1", ::Core::Keyboard::Key::_1 },
				{ "_2", ::Core::Keyboard::Key::_2 },
				{ "_3", ::Core::Keyboard::Key::_3 },
				{ "_4", ::Core::Keyboard::Key::_4 },
				{ "_5", ::Core::Keyboard::Key::_5 },
				{ "_6", ::Core::Keyboard::Key::_6 },
				{ "_7", ::Core::Keyboard::Key::_7 },
				{ "_8", ::Core::Keyboard::Key::_8 },
				{ "_9", ::Core::Keyboard::Key::_9 },
				{ "Semicolon", ::Core::Keyboard::Key::Semicolon },
				{ "Equal", ::Core::Keyboard::Key::Equal },
				{ "A", ::Core::Keyboard::Key::A },
				{ "B", ::Core::Keyboard::Key::B },
				{ "C", ::Core::Keyboard::Key::C },
				{ "D", ::Core::Keyboard::Key::D },
				{ "E", ::Core::Keyboard::Key::E },
				{ "F", ::Core::Keyboard::Key::F },
				{ "G", ::Core::Keyboard::Key::G },
				{ "H", ::Core::Keyboard::Key::H },
				{ "I", ::Core::Keyboard::Key::I },
				{ "J", ::Core::Keyboard::Key::J },
				{ "K", ::Core::Keyboard::Key::K },
				{ "L", ::Core::Keyboard::Key::L },
				{ "M", ::Core::Keyboard::Key::M },
				{ "N", ::Core::Keyboard::Key::N },
				{ "O", ::Core::Keyboard::Key::O },
				{ "P", ::Core::Keyboard::Key::P },
				{ "Q", ::Core::Keyboard::Key::Q },
				{ "R", ::Core::Keyboard::Key::R },
				{ "S", ::Core::Keyboard::Key::S },
				{ "T", ::Core::Keyboard::Key::T },
				{ "U", ::Core::Keyboard::Key::U },
				{ "V", ::Core::Keyboard::Key::V },
				{ "W", ::Core::Keyboard::Key::W },
				{ "X", ::Core::Keyboard::Key::X },
				{ "Y", ::Core::Keyboard::Key::Y },
				{ "Z", ::Core::Keyboard::Key::Z },
				{ "Left_Bracket", ::Core::Keyboard::Key::Left_Bracket },
				{ "Backslash", ::Core::Keyboard::Key::Backslash },
				{ "Right_Bracket", ::Core::Keyboard::Key::Right_Bracket },
				{ "Grave_Accent", ::Core::Keyboard::Key::Grave_Accent },
				{ "World_1", ::Core::Keyboard::Key::World_1 },
				{ "World_2", ::Core::Keyboard::Key::World_2 },
				{ "Escape", ::Core::Keyboard::Key::Escape },
				{ "Enter", ::Core::Keyboard::Key::Enter },
				{ "Tab", ::Core::Keyboard::Key::Tab },
				{ "Backspace", ::Core::Keyboard::Key::Backspace },
				{ "Insert", ::Core::Keyboard::Key::Insert },
				{ "Delete", ::Core::Keyboard::Key::Delete },
				{ "Right", ::Core::Keyboard::Key::Right },
				{ "Left", ::Core::Keyboard::Key::Left },
				{ "Down", ::Core::Keyboard::Key::Down },
				{ "Up", ::Core::Keyboard::Key::Up },
				{ "Page_Up", ::Core::Keyboard::Key::Page_Up },
				{ "Page_Down", ::Core::Keyboard::Key::Page_Down },
				{ "Home", ::Core::Keyboard::Key::Home },
				{ "End", ::Core::Keyboard::Key::End },
				{ "Caps_Lock", ::Core::Keyboard::Key::Caps_Lock },
				{ "Scroll_Lock", ::Core::Keyboard::Key::Scroll_Lock },
				{ "Num_Lock", ::Core::Keyboard::Key::Num_Lock },
				{ "Print_Screen", ::Core::Keyboard::Key::Print_Screen },
				{ "Pause", ::Core::Keyboard::Key::Pause },
				{ "F1", ::Core::Keyboard::Key::F1 },
				{ "F2", ::Core::Keyboard::Key::F2 },
				{ "F3", ::Core::Keyboard::Key::F3 },
				{ "F4", ::Core::Keyboard::Key::F4 },
				{ "F5", ::Core::Keyboard::Key::F5 },
				{ "F6", ::Core::Keyboard::Key::F6 },
				{ "F7", ::Core::Keyboard::Key::F7 },
				{ "F8", ::Core::Keyboard::Key::F8 },
				{ "F9", ::Core::Keyboard::Key::F9 },
				{ "F10", ::Core::Keyboard::Key::F10 },
				{ "F11", ::Core::Keyboard::Key::F11 },
				{ "F12", ::Core::Keyboard::Key::F12 },
				{ "F13", ::Core::Keyboard::Key::F13 },
				{ "F14", ::Core::Keyboard::Key::F14 },
				{ "F15", ::Core::Keyboard::Key::F15 },
				{ "F16", ::Core::Keyboard::Key::F16 },
				{ "F17", ::Core::Keyboard::Key::F17 },
				{ "F18", ::Core::Keyboard::Key::F18 },
				{ "F19", ::Core::Keyboard::Key::F19 },
				{ "F20", ::Core::Keyboard::Key::F20 },
				{ "F21", ::Core::Keyboard::Key::F21 },
				{ "F22", ::Core::Keyboard::Key::F22 },
				{ "F23", ::Core::Keyboard::Key::F23 },
				{ "F24", ::Core::Keyboard::Key::F24 },
				{ "F25", ::Core::Keyboard::Key::F25 },
				{ "Kp_0", ::Core::Keyboard::Key::Kp_0 },
				{ "Kp_1", ::Core::Keyboard::Key::Kp_1 },
				{ "Kp_2", ::Core::Keyboard::Key::Kp_2 },
				{ "Kp_3", ::Core::Keyboard::Key::Kp_3 },
				{ "Kp_4", ::Core::Keyboard::Key::Kp_4 },
				{ "Kp_5", ::Core::Keyboard::Key::Kp_5 },
				{ "Kp_6", ::Core::Keyboard::Key::Kp_6 },
				{ "Kp_7", ::Core::Keyboard::Key::Kp_7 },
				{ "Kp_8", ::Core::Keyboard::Key::Kp_8 },
				{ "Kp_9", ::Core::Keyboard::Key::Kp_9 },
				{ "Kp_Decimal", ::Core::Keyboard::Key::Kp_Decimal },
				{ "Kp_Divide", ::Core::Keyboard::Key::Kp_Divide },
				{ "Kp_Multiply", ::Core::Keyboard::Key::Kp_Multiply },
				{ "Kp_Subtract", ::Core::Keyboard::Key::Kp_Subtract },
				{ "Kp_Add", ::Core::Keyboard::Key::Kp_Add },
				{ "Kp_Enter", ::Core::Keyboard::Key::Kp_Enter },
				{ "Kp_Equal", ::Core::Keyboard::Key::Kp_Equal },
				{ "Left_Shift", ::Core::Keyboard::Key::Left_Shift },
				{ "Left_Control", ::Core::Keyboard::Key::Left_Control },
				{ "Left_Alt", ::Core::Keyboard::Key::Left_Alt },
				{ "Left_Super", ::Core::Keyboard::Key::Left_Super },
				{ "Right_Shift", ::Core::Keyboard::Key::Right_Shift },
				{ "Right_Control", ::Core::Keyboard::Key::Right_Control },
				{ "Right_Alt", ::Core::Keyboard::Key::Right_Alt },
				{ "Right_Super", ::Core::Keyboard::Key::Right_Super },
				{ "Menu", ::Core::Keyboard::Key::Menu },
				{ "Last", ::Core::Keyboard::Key::Last }
            };

            return map;
        }

    } // namespace Core::Keyboard::Key

    const std::string& ToString(::Core::Keyboard::Key value)
    {
        return Core::Keyboard::Key::ToString(value);
    }


} // namespace
