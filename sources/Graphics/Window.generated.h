/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

// clang-format off
#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

template<>
struct R<Core::DragAndDrop::State>
{
    static constexpr std::string_view Name() { return "State"; }
    static constexpr std::size_t Size() { return 3; }
    static constexpr std::string_view ParentScope() { return "Core::DragAndDrop"; }

    static std::string_view ToString(::Core::DragAndDrop::State value)
    {
        const auto& data = R<Core::DragAndDrop::State>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::DragAndDrop::State> FromString(std::string_view value)
    {
        const auto& data = R<Core::DragAndDrop::State>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::DragAndDrop::State, 3>& ToArrayC()
    {
        static constexpr std::array<::Core::DragAndDrop::State, 3> constants = {
				::Core::DragAndDrop::State::Idle,
				::Core::DragAndDrop::State::Started,
				::Core::DragAndDrop::State::Dragging
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 3>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 3> names = {
				std::string_view("Idle"),
				std::string_view("Started"),
				std::string_view("Dragging")
        };

        return names;
    }

    static const std::unordered_map<::Core::DragAndDrop::State, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::DragAndDrop::State, std::string_view> map = {
				{ ::Core::DragAndDrop::State::Idle, "Idle" },
				{ ::Core::DragAndDrop::State::Started, "Started" },
				{ ::Core::DragAndDrop::State::Dragging, "Dragging" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::DragAndDrop::State>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::DragAndDrop::State> map = {
				{ "Idle", ::Core::DragAndDrop::State::Idle },
				{ "Started", ::Core::DragAndDrop::State::Started },
				{ "Dragging", ::Core::DragAndDrop::State::Dragging }
        };

        return map;
    }
}; // struct R<Core::DragAndDrop::State>

template<>
struct R<Core::Window::CursorMode>
{
    static constexpr std::string_view Name() { return "CursorMode"; }
    static constexpr std::size_t Size() { return 3; }
    static constexpr std::string_view ParentScope() { return "Core::Window"; }

    static std::string_view ToString(::Core::Window::CursorMode value)
    {
        const auto& data = R<Core::Window::CursorMode>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::Window::CursorMode> FromString(std::string_view value)
    {
        const auto& data = R<Core::Window::CursorMode>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::Window::CursorMode, 3>& ToArrayC()
    {
        static constexpr std::array<::Core::Window::CursorMode, 3> constants = {
				::Core::Window::CursorMode::Normal,
				::Core::Window::CursorMode::Disabled,
				::Core::Window::CursorMode::Hidden
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 3>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 3> names = {
				std::string_view("Normal"),
				std::string_view("Disabled"),
				std::string_view("Hidden")
        };

        return names;
    }

    static const std::unordered_map<::Core::Window::CursorMode, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::Window::CursorMode, std::string_view> map = {
				{ ::Core::Window::CursorMode::Normal, "Normal" },
				{ ::Core::Window::CursorMode::Disabled, "Disabled" },
				{ ::Core::Window::CursorMode::Hidden, "Hidden" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::Window::CursorMode>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::Window::CursorMode> map = {
				{ "Normal", ::Core::Window::CursorMode::Normal },
				{ "Disabled", ::Core::Window::CursorMode::Disabled },
				{ "Hidden", ::Core::Window::CursorMode::Hidden }
        };

        return map;
    }
}; // struct R<Core::Window::CursorMode>

template<>
struct R<Core::Window>
{
    static constexpr std::string_view Name() { return "Window"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 4; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "ISize2", "_size" },
			{ "StringAtom", "_title" },
			{ "bool", "_isMaximized" },
			{ "bool", "_swapInterval" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Window& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Window>(obj);
        }

		s.write("_size", obj._size);
		s.write("_title", obj._title);
		s.write("_isMaximized", obj._isMaximized);
		s.write("_swapInterval", obj._swapInterval);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Window>(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Window& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Window>(obj);
        }

		s.write("_size", obj._size);
		s.write("_title", obj._title);
		s.write("_isMaximized", obj._isMaximized);
		s.write("_swapInterval", obj._swapInterval);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Window>(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Window& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Window>(obj);
        }

		s.read("_size", obj._size, 4);
		s.read("_title", obj._title, 4);
		s.read("_isMaximized", obj._isMaximized, false);
		s.read("_swapInterval", obj._swapInterval, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Window>(obj, s.logs());
        }
    }
}; // struct R<Core::Window>
// clang-format on
