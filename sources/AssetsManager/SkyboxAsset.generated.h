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
        static const std::string_view empty{};
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
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::array<std::filesystem::path, 6>", "_paths" },
			{ "bool", "_isFlipVertically" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::SkyboxAsset& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseAsset>::Serialize<RImpl>(obj).getData());
		s.write("_paths", obj._paths);
		s.write("_isFlipVertically", obj._isFlipVertically);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::SkyboxAsset& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseAsset>::Serialize<RImpl>(obj).getData());
		s.write("_paths", obj._paths);
		s.write("_isFlipVertically", obj._isFlipVertically);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::SkyboxAsset& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Core::BaseAsset>::Deserialize<RImpl>(s, obj, true);
		s.read("_paths", obj._paths, 4);
		s.read("_isFlipVertically", obj._isFlipVertically, false);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Core::SkyboxAsset>
// clang-format on
