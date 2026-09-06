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
struct R<Core::GameInstance::RenderMode>
{
    static constexpr std::string_view Name() { return "RenderMode"; }
    static constexpr std::string_view FullName() { return "Core::GameInstance::RenderMode"; }
    static constexpr std::size_t Size() { return 2; }
    static constexpr std::string_view ParentScope() { return "Core::GameInstance"; }

    static std::string_view ToString(::Core::GameInstance::RenderMode value)
    {
        const auto& data = R<Core::GameInstance::RenderMode>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static constexpr std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::GameInstance::RenderMode> FromString(std::string_view value)
    {
        const auto& data = R<Core::GameInstance::RenderMode>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(::Core::GameInstance::RenderMode value, RResourceStream<RImpl>& s)
    {
        s.getData() = ToString(value);
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static RResourceStream<RImpl> Serialize(::Core::GameInstance::RenderMode value)
    {
        RResourceStream<RImpl> s;
        Serialize<RImpl>(value, s);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, ::Core::GameInstance::RenderMode& value)
    {
        auto tmp = FromString(s.getData().template get<std::string>());
        if (tmp.has_value())
            value = tmp.value();
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static ::Core::GameInstance::RenderMode Deserialize(const RResourceStream<RImpl>& s)
    {
        ::Core::GameInstance::RenderMode out;
        Deserialize<RImpl>(s, out);
        return out;
    }

    static constexpr const std::array<::Core::GameInstance::RenderMode, 2>& ToArrayC()
    {
        static constexpr std::array<::Core::GameInstance::RenderMode, 2> constants = {
				::Core::GameInstance::RenderMode::GameOnly,
				::Core::GameInstance::RenderMode::Editor
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 2>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 2> names = {
				std::string_view("GameOnly"),
				std::string_view("Editor")
        };

        return names;
    }

    static const std::unordered_map<::Core::GameInstance::RenderMode, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::GameInstance::RenderMode, std::string_view> map = {
				{ ::Core::GameInstance::RenderMode::GameOnly, "GameOnly" },
				{ ::Core::GameInstance::RenderMode::Editor, "Editor" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::GameInstance::RenderMode>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::GameInstance::RenderMode> map = {
				{ "GameOnly", ::Core::GameInstance::RenderMode::GameOnly },
				{ "Editor", ::Core::GameInstance::RenderMode::Editor }
        };

        return map;
    }
}; // struct R<Core::GameInstance::RenderMode>

// clang-format on
