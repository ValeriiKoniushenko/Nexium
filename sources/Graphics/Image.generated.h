/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

#include <array>
#include <optional>
#include <string>
#include <unordered_map>

template<>
struct R<Core::Image::Channel>
{
    static constexpr std::string_view Name() { return "Channel"; }
    static constexpr std::size_t Size() { return 5; }
    static constexpr std::string_view ParentScope() { return "Core::Image"; }

    static std::string_view ToString(::Core::Image::Channel value)
    {
        const auto& data = R<Core::Image::Channel>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::Image::Channel> FromString(std::string_view value)
    {
        const auto& data = R<Core::Image::Channel>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::Image::Channel, 5>& ToArrayC()
    {
        static constexpr std::array<::Core::Image::Channel, 5> constants = {
				::Core::Image::Channel::None,
				::Core::Image::Channel::Grey,
				::Core::Image::Channel::GreyA,
				::Core::Image::Channel::RGB,
				::Core::Image::Channel::RGBA
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 5>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 5> names = {
				std::string_view("None"),
				std::string_view("Grey"),
				std::string_view("GreyA"),
				std::string_view("RGB"),
				std::string_view("RGBA")
        };

        return names;
    }

    static const std::unordered_map<::Core::Image::Channel, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::Image::Channel, std::string_view> map = {
				{ ::Core::Image::Channel::None, "None" },
				{ ::Core::Image::Channel::Grey, "Grey" },
				{ ::Core::Image::Channel::GreyA, "GreyA" },
				{ ::Core::Image::Channel::RGB, "RGB" },
				{ ::Core::Image::Channel::RGBA, "RGBA" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::Image::Channel>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::Image::Channel> map = {
				{ "None", ::Core::Image::Channel::None },
				{ "Grey", ::Core::Image::Channel::Grey },
				{ "GreyA", ::Core::Image::Channel::GreyA },
				{ "RGB", ::Core::Image::Channel::RGB },
				{ "RGBA", ::Core::Image::Channel::RGBA }
        };

        return map;
    }
}; // struct R<Core::Image::Channel>

