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
#include <unordered_map>

template<>
struct R<Core::ViewportMode>
{
    static constexpr std::string_view Name() { return "ViewportMode"; }
    static constexpr std::string_view FullName() { return "Core::ViewportMode"; }
    static constexpr std::size_t Size() { return 2; }
    static constexpr std::string_view ParentScope() { return "Core"; }

    static std::string_view ToString(::Core::ViewportMode value)
    {
        const auto& data = R<Core::ViewportMode>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static constexpr std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::ViewportMode> FromString(std::string_view value)
    {
        const auto& data = R<Core::ViewportMode>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(::Core::ViewportMode value, RResourceStream<RImpl>& s)
    {
        s.getData() = ToString(value);
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static RResourceStream<RImpl> Serialize(::Core::ViewportMode value)
    {
        RResourceStream<RImpl> s;
        Serialize<RImpl>(value, s);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, ::Core::ViewportMode& value)
    {
        auto tmp = FromString(s.getData().template get<std::string>());
        if (tmp.has_value())
            value = tmp.value();
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static ::Core::ViewportMode Deserialize(const RResourceStream<RImpl>& s)
    {
        ::Core::ViewportMode out;
        Deserialize<RImpl>(s, out);
        return out;
    }

    static constexpr const std::array<::Core::ViewportMode, 2>& ToArrayC()
    {
        static constexpr std::array<::Core::ViewportMode, 2> constants = {
				::Core::ViewportMode::Default,
				::Core::ViewportMode::ZoomIn
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 2>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 2> names = {
				std::string_view("Default"),
				std::string_view("ZoomIn")
        };

        return names;
    }

    static const std::unordered_map<::Core::ViewportMode, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::ViewportMode, std::string_view> map = {
				{ ::Core::ViewportMode::Default, "Default" },
				{ ::Core::ViewportMode::ZoomIn, "ZoomIn" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::ViewportMode>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::ViewportMode> map = {
				{ "Default", ::Core::ViewportMode::Default },
				{ "ZoomIn", ::Core::ViewportMode::ZoomIn }
        };

        return map;
    }
}; // struct R<Core::ViewportMode>

// clang-format on
