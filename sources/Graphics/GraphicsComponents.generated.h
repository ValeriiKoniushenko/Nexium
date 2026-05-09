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
struct R<Core::InterleavedGraphicsData::ModifiedValue>
{
    static constexpr std::string_view Name() { return "ModifiedValue"; }
    static constexpr std::size_t Size() { return 20; }
    static constexpr std::string_view ParentScope() { return "Core::InterleavedGraphicsData"; }

    static std::string_view ToString(::Core::InterleavedGraphicsData::ModifiedValue value)
    {
        const auto& data = R<Core::InterleavedGraphicsData::ModifiedValue>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::InterleavedGraphicsData::ModifiedValue> FromString(std::string_view value)
    {
        const auto& data = R<Core::InterleavedGraphicsData::ModifiedValue>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::InterleavedGraphicsData::ModifiedValue, 20>& ToArrayC()
    {
        static constexpr std::array<::Core::InterleavedGraphicsData::ModifiedValue, 20> constants = {
				::Core::InterleavedGraphicsData::ModifiedValue::None,
				::Core::InterleavedGraphicsData::ModifiedValue::Blend,
				::Core::InterleavedGraphicsData::ModifiedValue::CullFace,
				::Core::InterleavedGraphicsData::ModifiedValue::DepthTest,
				::Core::InterleavedGraphicsData::ModifiedValue::Dither,
				::Core::InterleavedGraphicsData::ModifiedValue::PolygonOffsetFill,
				::Core::InterleavedGraphicsData::ModifiedValue::PolygonOffsetLine,
				::Core::InterleavedGraphicsData::ModifiedValue::PolygonOffsetPoint,
				::Core::InterleavedGraphicsData::ModifiedValue::PrimitiveRestart,
				::Core::InterleavedGraphicsData::ModifiedValue::PrimitiveRestartFixedIndex,
				::Core::InterleavedGraphicsData::ModifiedValue::RasterizerDiscard,
				::Core::InterleavedGraphicsData::ModifiedValue::SampleAlphaToCoverage,
				::Core::InterleavedGraphicsData::ModifiedValue::SampleAlphaToOne,
				::Core::InterleavedGraphicsData::ModifiedValue::SampleCoverage,
				::Core::InterleavedGraphicsData::ModifiedValue::SampleMask,
				::Core::InterleavedGraphicsData::ModifiedValue::ScissorTest,
				::Core::InterleavedGraphicsData::ModifiedValue::StencilTest,
				::Core::InterleavedGraphicsData::ModifiedValue::Multisample,
				::Core::InterleavedGraphicsData::ModifiedValue::FramebufferSrgb,
				::Core::InterleavedGraphicsData::ModifiedValue::ProgramPointSize
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 20>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 20> names = {
				std::string_view("None"),
				std::string_view("Blend"),
				std::string_view("CullFace"),
				std::string_view("DepthTest"),
				std::string_view("Dither"),
				std::string_view("PolygonOffsetFill"),
				std::string_view("PolygonOffsetLine"),
				std::string_view("PolygonOffsetPoint"),
				std::string_view("PrimitiveRestart"),
				std::string_view("PrimitiveRestartFixedIndex"),
				std::string_view("RasterizerDiscard"),
				std::string_view("SampleAlphaToCoverage"),
				std::string_view("SampleAlphaToOne"),
				std::string_view("SampleCoverage"),
				std::string_view("SampleMask"),
				std::string_view("ScissorTest"),
				std::string_view("StencilTest"),
				std::string_view("Multisample"),
				std::string_view("FramebufferSrgb"),
				std::string_view("ProgramPointSize")
        };

        return names;
    }

    static const std::unordered_map<::Core::InterleavedGraphicsData::ModifiedValue, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::InterleavedGraphicsData::ModifiedValue, std::string_view> map = {
				{ ::Core::InterleavedGraphicsData::ModifiedValue::None, "None" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::Blend, "Blend" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::CullFace, "CullFace" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::DepthTest, "DepthTest" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::Dither, "Dither" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::PolygonOffsetFill, "PolygonOffsetFill" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::PolygonOffsetLine, "PolygonOffsetLine" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::PolygonOffsetPoint, "PolygonOffsetPoint" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::PrimitiveRestart, "PrimitiveRestart" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::PrimitiveRestartFixedIndex, "PrimitiveRestartFixedIndex" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::RasterizerDiscard, "RasterizerDiscard" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::SampleAlphaToCoverage, "SampleAlphaToCoverage" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::SampleAlphaToOne, "SampleAlphaToOne" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::SampleCoverage, "SampleCoverage" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::SampleMask, "SampleMask" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::ScissorTest, "ScissorTest" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::StencilTest, "StencilTest" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::Multisample, "Multisample" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::FramebufferSrgb, "FramebufferSrgb" },
				{ ::Core::InterleavedGraphicsData::ModifiedValue::ProgramPointSize, "ProgramPointSize" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::InterleavedGraphicsData::ModifiedValue>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::InterleavedGraphicsData::ModifiedValue> map = {
				{ "None", ::Core::InterleavedGraphicsData::ModifiedValue::None },
				{ "Blend", ::Core::InterleavedGraphicsData::ModifiedValue::Blend },
				{ "CullFace", ::Core::InterleavedGraphicsData::ModifiedValue::CullFace },
				{ "DepthTest", ::Core::InterleavedGraphicsData::ModifiedValue::DepthTest },
				{ "Dither", ::Core::InterleavedGraphicsData::ModifiedValue::Dither },
				{ "PolygonOffsetFill", ::Core::InterleavedGraphicsData::ModifiedValue::PolygonOffsetFill },
				{ "PolygonOffsetLine", ::Core::InterleavedGraphicsData::ModifiedValue::PolygonOffsetLine },
				{ "PolygonOffsetPoint", ::Core::InterleavedGraphicsData::ModifiedValue::PolygonOffsetPoint },
				{ "PrimitiveRestart", ::Core::InterleavedGraphicsData::ModifiedValue::PrimitiveRestart },
				{ "PrimitiveRestartFixedIndex", ::Core::InterleavedGraphicsData::ModifiedValue::PrimitiveRestartFixedIndex },
				{ "RasterizerDiscard", ::Core::InterleavedGraphicsData::ModifiedValue::RasterizerDiscard },
				{ "SampleAlphaToCoverage", ::Core::InterleavedGraphicsData::ModifiedValue::SampleAlphaToCoverage },
				{ "SampleAlphaToOne", ::Core::InterleavedGraphicsData::ModifiedValue::SampleAlphaToOne },
				{ "SampleCoverage", ::Core::InterleavedGraphicsData::ModifiedValue::SampleCoverage },
				{ "SampleMask", ::Core::InterleavedGraphicsData::ModifiedValue::SampleMask },
				{ "ScissorTest", ::Core::InterleavedGraphicsData::ModifiedValue::ScissorTest },
				{ "StencilTest", ::Core::InterleavedGraphicsData::ModifiedValue::StencilTest },
				{ "Multisample", ::Core::InterleavedGraphicsData::ModifiedValue::Multisample },
				{ "FramebufferSrgb", ::Core::InterleavedGraphicsData::ModifiedValue::FramebufferSrgb },
				{ "ProgramPointSize", ::Core::InterleavedGraphicsData::ModifiedValue::ProgramPointSize }
        };

        return map;
    }
}; // struct R<Core::InterleavedGraphicsData::ModifiedValue>

template<>
struct R<Core::InterleavedGraphicsData::Modifier>
{
    static constexpr std::string_view Name() { return "Modifier"; }
    static constexpr std::size_t Size() { return 3; }
    static constexpr std::string_view ParentScope() { return "Core::InterleavedGraphicsData"; }

    static std::string_view ToString(::Core::InterleavedGraphicsData::Modifier value)
    {
        const auto& data = R<Core::InterleavedGraphicsData::Modifier>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::InterleavedGraphicsData::Modifier> FromString(std::string_view value)
    {
        const auto& data = R<Core::InterleavedGraphicsData::Modifier>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::InterleavedGraphicsData::Modifier, 3>& ToArrayC()
    {
        static constexpr std::array<::Core::InterleavedGraphicsData::Modifier, 3> constants = {
				::Core::InterleavedGraphicsData::Modifier::None,
				::Core::InterleavedGraphicsData::Modifier::Enable,
				::Core::InterleavedGraphicsData::Modifier::Disable
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 3>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 3> names = {
				std::string_view("None"),
				std::string_view("Enable"),
				std::string_view("Disable")
        };

        return names;
    }

    static const std::unordered_map<::Core::InterleavedGraphicsData::Modifier, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::InterleavedGraphicsData::Modifier, std::string_view> map = {
				{ ::Core::InterleavedGraphicsData::Modifier::None, "None" },
				{ ::Core::InterleavedGraphicsData::Modifier::Enable, "Enable" },
				{ ::Core::InterleavedGraphicsData::Modifier::Disable, "Disable" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::InterleavedGraphicsData::Modifier>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::InterleavedGraphicsData::Modifier> map = {
				{ "None", ::Core::InterleavedGraphicsData::Modifier::None },
				{ "Enable", ::Core::InterleavedGraphicsData::Modifier::Enable },
				{ "Disable", ::Core::InterleavedGraphicsData::Modifier::Disable }
        };

        return map;
    }
}; // struct R<Core::InterleavedGraphicsData::Modifier>

template<>
struct R<Core::InterleavedGraphicsData>
{
    static constexpr std::string_view Name() { return "InterleavedGraphicsData"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::vector<ModifierParam>", "_drawModifiers" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::InterleavedGraphicsData& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write("_drawModifiers", obj._drawModifiers);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::InterleavedGraphicsData& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write("_drawModifiers", obj._drawModifiers);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::InterleavedGraphicsData& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		s.read("_drawModifiers", obj._drawModifiers, 4);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Core::InterleavedGraphicsData>
// clang-format on
