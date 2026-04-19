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
struct R<Core::GraphicsComponentData::ModifiedValue>
{
    static constexpr std::string_view Name() { return "ModifiedValue"; }
    static constexpr std::size_t Size() { return 20; }
    static constexpr std::string_view ParentScope() { return "Core::GraphicsComponentData"; }

    static std::string_view ToString(::Core::GraphicsComponentData::ModifiedValue value)
    {
        const auto& data = R<Core::GraphicsComponentData::ModifiedValue>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::GraphicsComponentData::ModifiedValue> FromString(std::string_view value)
    {
        const auto& data = R<Core::GraphicsComponentData::ModifiedValue>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::GraphicsComponentData::ModifiedValue, 20>& ToArrayC()
    {
        static constexpr std::array<::Core::GraphicsComponentData::ModifiedValue, 20> constants = {
				::Core::GraphicsComponentData::ModifiedValue::None,
				::Core::GraphicsComponentData::ModifiedValue::Blend,
				::Core::GraphicsComponentData::ModifiedValue::CullFace,
				::Core::GraphicsComponentData::ModifiedValue::DepthTest,
				::Core::GraphicsComponentData::ModifiedValue::Dither,
				::Core::GraphicsComponentData::ModifiedValue::PolygonOffsetFill,
				::Core::GraphicsComponentData::ModifiedValue::PolygonOffsetLine,
				::Core::GraphicsComponentData::ModifiedValue::PolygonOffsetPoint,
				::Core::GraphicsComponentData::ModifiedValue::PrimitiveRestart,
				::Core::GraphicsComponentData::ModifiedValue::PrimitiveRestartFixedIndex,
				::Core::GraphicsComponentData::ModifiedValue::RasterizerDiscard,
				::Core::GraphicsComponentData::ModifiedValue::SampleAlphaToCoverage,
				::Core::GraphicsComponentData::ModifiedValue::SampleAlphaToOne,
				::Core::GraphicsComponentData::ModifiedValue::SampleCoverage,
				::Core::GraphicsComponentData::ModifiedValue::SampleMask,
				::Core::GraphicsComponentData::ModifiedValue::ScissorTest,
				::Core::GraphicsComponentData::ModifiedValue::StencilTest,
				::Core::GraphicsComponentData::ModifiedValue::Multisample,
				::Core::GraphicsComponentData::ModifiedValue::FramebufferSrgb,
				::Core::GraphicsComponentData::ModifiedValue::ProgramPointSize
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

    static const std::unordered_map<::Core::GraphicsComponentData::ModifiedValue, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::GraphicsComponentData::ModifiedValue, std::string_view> map = {
				{ ::Core::GraphicsComponentData::ModifiedValue::None, "None" },
				{ ::Core::GraphicsComponentData::ModifiedValue::Blend, "Blend" },
				{ ::Core::GraphicsComponentData::ModifiedValue::CullFace, "CullFace" },
				{ ::Core::GraphicsComponentData::ModifiedValue::DepthTest, "DepthTest" },
				{ ::Core::GraphicsComponentData::ModifiedValue::Dither, "Dither" },
				{ ::Core::GraphicsComponentData::ModifiedValue::PolygonOffsetFill, "PolygonOffsetFill" },
				{ ::Core::GraphicsComponentData::ModifiedValue::PolygonOffsetLine, "PolygonOffsetLine" },
				{ ::Core::GraphicsComponentData::ModifiedValue::PolygonOffsetPoint, "PolygonOffsetPoint" },
				{ ::Core::GraphicsComponentData::ModifiedValue::PrimitiveRestart, "PrimitiveRestart" },
				{ ::Core::GraphicsComponentData::ModifiedValue::PrimitiveRestartFixedIndex, "PrimitiveRestartFixedIndex" },
				{ ::Core::GraphicsComponentData::ModifiedValue::RasterizerDiscard, "RasterizerDiscard" },
				{ ::Core::GraphicsComponentData::ModifiedValue::SampleAlphaToCoverage, "SampleAlphaToCoverage" },
				{ ::Core::GraphicsComponentData::ModifiedValue::SampleAlphaToOne, "SampleAlphaToOne" },
				{ ::Core::GraphicsComponentData::ModifiedValue::SampleCoverage, "SampleCoverage" },
				{ ::Core::GraphicsComponentData::ModifiedValue::SampleMask, "SampleMask" },
				{ ::Core::GraphicsComponentData::ModifiedValue::ScissorTest, "ScissorTest" },
				{ ::Core::GraphicsComponentData::ModifiedValue::StencilTest, "StencilTest" },
				{ ::Core::GraphicsComponentData::ModifiedValue::Multisample, "Multisample" },
				{ ::Core::GraphicsComponentData::ModifiedValue::FramebufferSrgb, "FramebufferSrgb" },
				{ ::Core::GraphicsComponentData::ModifiedValue::ProgramPointSize, "ProgramPointSize" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::GraphicsComponentData::ModifiedValue>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::GraphicsComponentData::ModifiedValue> map = {
				{ "None", ::Core::GraphicsComponentData::ModifiedValue::None },
				{ "Blend", ::Core::GraphicsComponentData::ModifiedValue::Blend },
				{ "CullFace", ::Core::GraphicsComponentData::ModifiedValue::CullFace },
				{ "DepthTest", ::Core::GraphicsComponentData::ModifiedValue::DepthTest },
				{ "Dither", ::Core::GraphicsComponentData::ModifiedValue::Dither },
				{ "PolygonOffsetFill", ::Core::GraphicsComponentData::ModifiedValue::PolygonOffsetFill },
				{ "PolygonOffsetLine", ::Core::GraphicsComponentData::ModifiedValue::PolygonOffsetLine },
				{ "PolygonOffsetPoint", ::Core::GraphicsComponentData::ModifiedValue::PolygonOffsetPoint },
				{ "PrimitiveRestart", ::Core::GraphicsComponentData::ModifiedValue::PrimitiveRestart },
				{ "PrimitiveRestartFixedIndex", ::Core::GraphicsComponentData::ModifiedValue::PrimitiveRestartFixedIndex },
				{ "RasterizerDiscard", ::Core::GraphicsComponentData::ModifiedValue::RasterizerDiscard },
				{ "SampleAlphaToCoverage", ::Core::GraphicsComponentData::ModifiedValue::SampleAlphaToCoverage },
				{ "SampleAlphaToOne", ::Core::GraphicsComponentData::ModifiedValue::SampleAlphaToOne },
				{ "SampleCoverage", ::Core::GraphicsComponentData::ModifiedValue::SampleCoverage },
				{ "SampleMask", ::Core::GraphicsComponentData::ModifiedValue::SampleMask },
				{ "ScissorTest", ::Core::GraphicsComponentData::ModifiedValue::ScissorTest },
				{ "StencilTest", ::Core::GraphicsComponentData::ModifiedValue::StencilTest },
				{ "Multisample", ::Core::GraphicsComponentData::ModifiedValue::Multisample },
				{ "FramebufferSrgb", ::Core::GraphicsComponentData::ModifiedValue::FramebufferSrgb },
				{ "ProgramPointSize", ::Core::GraphicsComponentData::ModifiedValue::ProgramPointSize }
        };

        return map;
    }
}; // struct R<Core::GraphicsComponentData::ModifiedValue>

template<>
struct R<Core::GraphicsComponentData::Modifier>
{
    static constexpr std::string_view Name() { return "Modifier"; }
    static constexpr std::size_t Size() { return 3; }
    static constexpr std::string_view ParentScope() { return "Core::GraphicsComponentData"; }

    static std::string_view ToString(::Core::GraphicsComponentData::Modifier value)
    {
        const auto& data = R<Core::GraphicsComponentData::Modifier>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::GraphicsComponentData::Modifier> FromString(std::string_view value)
    {
        const auto& data = R<Core::GraphicsComponentData::Modifier>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::GraphicsComponentData::Modifier, 3>& ToArrayC()
    {
        static constexpr std::array<::Core::GraphicsComponentData::Modifier, 3> constants = {
				::Core::GraphicsComponentData::Modifier::None,
				::Core::GraphicsComponentData::Modifier::Enable,
				::Core::GraphicsComponentData::Modifier::Disable
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

    static const std::unordered_map<::Core::GraphicsComponentData::Modifier, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::GraphicsComponentData::Modifier, std::string_view> map = {
				{ ::Core::GraphicsComponentData::Modifier::None, "None" },
				{ ::Core::GraphicsComponentData::Modifier::Enable, "Enable" },
				{ ::Core::GraphicsComponentData::Modifier::Disable, "Disable" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::GraphicsComponentData::Modifier>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::GraphicsComponentData::Modifier> map = {
				{ "None", ::Core::GraphicsComponentData::Modifier::None },
				{ "Enable", ::Core::GraphicsComponentData::Modifier::Enable },
				{ "Disable", ::Core::GraphicsComponentData::Modifier::Disable }
        };

        return map;
    }
}; // struct R<Core::GraphicsComponentData::Modifier>

template<>
struct R<Core::GraphicsComponentData>
{
    static constexpr std::string_view Name() { return "GraphicsComponentData"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::vector<ModifierParam>", "_drawModifiers" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::GraphicsComponentData& obj, bool noSignals = false)
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
    static void Serialize(const Core::GraphicsComponentData& obj, RResourceStream<RImpl>& s, bool noSignals = false)
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
    static void Deserialize(const RResourceStream<RImpl>& s, Core::GraphicsComponentData& obj, bool noSignals = false)
    {if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		s.read("_drawModifiers", obj._drawModifiers, 4);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Core::GraphicsComponentData>
// clang-format on
