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
struct R<Core::SkyboxAsset::Direction>
{
    static constexpr std::string_view Name() { return "Direction"; }
    static constexpr std::string_view FullName() { return "Core::SkyboxAsset::Direction"; }
    static constexpr std::size_t Size() { return 6; }
    static constexpr std::string_view ParentScope() { return "Core::SkyboxAsset"; }

    static std::string_view ToString(::Core::SkyboxAsset::Direction value)
    {
        const auto& data = R<Core::SkyboxAsset::Direction>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static constexpr std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::SkyboxAsset::Direction> FromString(std::string_view value)
    {
        const auto& data = R<Core::SkyboxAsset::Direction>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(::Core::SkyboxAsset::Direction value, RResourceStream<RImpl>& s)
    {
        s.getData() = ToString(value);
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static RResourceStream<RImpl> Serialize(::Core::SkyboxAsset::Direction value)
    {
        RResourceStream<RImpl> s;
        Serialize<RImpl>(value, s);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, ::Core::SkyboxAsset::Direction& value)
    {
        auto tmp = FromString(s.getData().template get<std::string>());
        if (tmp.has_value())
            value = tmp.value();
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static ::Core::SkyboxAsset::Direction Deserialize(const RResourceStream<RImpl>& s)
    {
        ::Core::SkyboxAsset::Direction out;
        Deserialize<RImpl>(s, out);
        return out;
    }

    static constexpr const std::array<::Core::SkyboxAsset::Direction, 6>& ToArrayC()
    {
        static constexpr std::array<::Core::SkyboxAsset::Direction, 6> constants = {
				::Core::SkyboxAsset::Direction::Right,
				::Core::SkyboxAsset::Direction::Left,
				::Core::SkyboxAsset::Direction::Top,
				::Core::SkyboxAsset::Direction::Bottom,
				::Core::SkyboxAsset::Direction::Front,
				::Core::SkyboxAsset::Direction::Back
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 6>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 6> names = {
				std::string_view("Right"),
				std::string_view("Left"),
				std::string_view("Top"),
				std::string_view("Bottom"),
				std::string_view("Front"),
				std::string_view("Back")
        };

        return names;
    }

    static const std::unordered_map<::Core::SkyboxAsset::Direction, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::SkyboxAsset::Direction, std::string_view> map = {
				{ ::Core::SkyboxAsset::Direction::Right, "Right" },
				{ ::Core::SkyboxAsset::Direction::Left, "Left" },
				{ ::Core::SkyboxAsset::Direction::Top, "Top" },
				{ ::Core::SkyboxAsset::Direction::Bottom, "Bottom" },
				{ ::Core::SkyboxAsset::Direction::Front, "Front" },
				{ ::Core::SkyboxAsset::Direction::Back, "Back" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::SkyboxAsset::Direction>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::SkyboxAsset::Direction> map = {
				{ "Right", ::Core::SkyboxAsset::Direction::Right },
				{ "Left", ::Core::SkyboxAsset::Direction::Left },
				{ "Top", ::Core::SkyboxAsset::Direction::Top },
				{ "Bottom", ::Core::SkyboxAsset::Direction::Bottom },
				{ "Front", ::Core::SkyboxAsset::Direction::Front },
				{ "Back", ::Core::SkyboxAsset::Direction::Back }
        };

        return map;
    }
}; // struct R<Core::SkyboxAsset::Direction>

template<>
struct R<Core::SkyboxAsset>
{
    static constexpr std::string_view Name() { return "SkyboxAsset"; }
    static constexpr std::string_view FullName() { return "Core::SkyboxAsset"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::array<std::filesystem::path, 6>", "_paths", {} },
			{ "bool", "_isFlipVertically", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::SkyboxAsset& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::SkyboxAsset>(obj);
        }

		s.write(R<Core::BaseAsset>::Serialize<RImpl>(obj).getData());
		s.write("_paths", obj._paths);
		s.write("_isFlipVertically", obj._isFlipVertically);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::SkyboxAsset>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::SkyboxAsset& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::SkyboxAsset& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::SkyboxAsset>(obj);
        }

		R<Core::BaseAsset>::Deserialize<RImpl>(s, obj, true);
		s.read("_paths", obj._paths, decltype(obj._paths){}, 16);
		s.read("_isFlipVertically", obj._isFlipVertically, false, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::SkyboxAsset>(obj, s.logs());
        }
    }

    static bool GetField(Core::SkyboxAsset& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_paths") == 0)
        {
            onFound((&obj._paths), "std::array<std::filesystem::path, 6>");
            return true;
        }
        if (std::strcmp(fieldName, "_isFlipVertically") == 0)
        {
            onFound((&obj._isFlipVertically), "bool");
            return true;
        }

		return false;
    }
}; // struct R<Core::SkyboxAsset>
// clang-format on
