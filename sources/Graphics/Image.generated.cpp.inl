/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

namespace R
{

    namespace Core::Image::Channel
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "Channel"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::Image"; return name; }

        const std::string& ToString(::Core::Image::Channel value)
        {
            const auto& data = R::Core::Image::Channel::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static constexpr std::string empty{};
            return empty;
        }

        std::optional<::Core::Image::Channel> FromString(const std::string& value)
        {
            const auto& data = R::Core::Image::Channel::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::Image::Channel, 5>& ToArrayC()
        {
            static const std::array<::Core::Image::Channel, 5> constants = {
				::Core::Image::Channel::None,
				::Core::Image::Channel::Grey,
				::Core::Image::Channel::GreyA,
				::Core::Image::Channel::RGB,
				::Core::Image::Channel::RGBA
            };

            return constants;
        }

        const std::array<std::string, 5>& ToArrayN()
        {
            static const std::array<std::string, 5> names = {
				std::string("None"),
				std::string("Grey"),
				std::string("GreyA"),
				std::string("RGB"),
				std::string("RGBA")
            };

            return names;
        }

        const std::unordered_map<::Core::Image::Channel, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::Image::Channel, std::string> map = {
				{ ::Core::Image::Channel::None, "None" },
				{ ::Core::Image::Channel::Grey, "Grey" },
				{ ::Core::Image::Channel::GreyA, "GreyA" },
				{ ::Core::Image::Channel::RGB, "RGB" },
				{ ::Core::Image::Channel::RGBA, "RGBA" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::Image::Channel>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::Image::Channel> map = {
				{ "None", ::Core::Image::Channel::None },
				{ "Grey", ::Core::Image::Channel::Grey },
				{ "GreyA", ::Core::Image::Channel::GreyA },
				{ "RGB", ::Core::Image::Channel::RGB },
				{ "RGBA", ::Core::Image::Channel::RGBA }
            };

            return map;
        }

    } // namespace Core::Image::Channel

    const std::string& ToString(::Core::Image::Channel value)
    {
        return Core::Image::Channel::ToString(value);
    }


} // namespace
