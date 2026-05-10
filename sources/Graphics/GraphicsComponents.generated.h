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
struct R<Core::BaseGraphicsData::ModifiedValue>
{
    static constexpr std::string_view Name() { return "ModifiedValue"; }
    static constexpr std::size_t Size() { return 20; }
    static constexpr std::string_view ParentScope() { return "Core::BaseGraphicsData"; }

    static std::string_view ToString(::Core::BaseGraphicsData::ModifiedValue value)
    {
        const auto& data = R<Core::BaseGraphicsData::ModifiedValue>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::BaseGraphicsData::ModifiedValue> FromString(std::string_view value)
    {
        const auto& data = R<Core::BaseGraphicsData::ModifiedValue>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::BaseGraphicsData::ModifiedValue, 20>& ToArrayC()
    {
        static constexpr std::array<::Core::BaseGraphicsData::ModifiedValue, 20> constants = {
				::Core::BaseGraphicsData::ModifiedValue::None,
				::Core::BaseGraphicsData::ModifiedValue::Blend,
				::Core::BaseGraphicsData::ModifiedValue::CullFace,
				::Core::BaseGraphicsData::ModifiedValue::DepthTest,
				::Core::BaseGraphicsData::ModifiedValue::Dither,
				::Core::BaseGraphicsData::ModifiedValue::PolygonOffsetFill,
				::Core::BaseGraphicsData::ModifiedValue::PolygonOffsetLine,
				::Core::BaseGraphicsData::ModifiedValue::PolygonOffsetPoint,
				::Core::BaseGraphicsData::ModifiedValue::PrimitiveRestart,
				::Core::BaseGraphicsData::ModifiedValue::PrimitiveRestartFixedIndex,
				::Core::BaseGraphicsData::ModifiedValue::RasterizerDiscard,
				::Core::BaseGraphicsData::ModifiedValue::SampleAlphaToCoverage,
				::Core::BaseGraphicsData::ModifiedValue::SampleAlphaToOne,
				::Core::BaseGraphicsData::ModifiedValue::SampleCoverage,
				::Core::BaseGraphicsData::ModifiedValue::SampleMask,
				::Core::BaseGraphicsData::ModifiedValue::ScissorTest,
				::Core::BaseGraphicsData::ModifiedValue::StencilTest,
				::Core::BaseGraphicsData::ModifiedValue::Multisample,
				::Core::BaseGraphicsData::ModifiedValue::FramebufferSrgb,
				::Core::BaseGraphicsData::ModifiedValue::ProgramPointSize
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

    static const std::unordered_map<::Core::BaseGraphicsData::ModifiedValue, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::BaseGraphicsData::ModifiedValue, std::string_view> map = {
				{ ::Core::BaseGraphicsData::ModifiedValue::None, "None" },
				{ ::Core::BaseGraphicsData::ModifiedValue::Blend, "Blend" },
				{ ::Core::BaseGraphicsData::ModifiedValue::CullFace, "CullFace" },
				{ ::Core::BaseGraphicsData::ModifiedValue::DepthTest, "DepthTest" },
				{ ::Core::BaseGraphicsData::ModifiedValue::Dither, "Dither" },
				{ ::Core::BaseGraphicsData::ModifiedValue::PolygonOffsetFill, "PolygonOffsetFill" },
				{ ::Core::BaseGraphicsData::ModifiedValue::PolygonOffsetLine, "PolygonOffsetLine" },
				{ ::Core::BaseGraphicsData::ModifiedValue::PolygonOffsetPoint, "PolygonOffsetPoint" },
				{ ::Core::BaseGraphicsData::ModifiedValue::PrimitiveRestart, "PrimitiveRestart" },
				{ ::Core::BaseGraphicsData::ModifiedValue::PrimitiveRestartFixedIndex, "PrimitiveRestartFixedIndex" },
				{ ::Core::BaseGraphicsData::ModifiedValue::RasterizerDiscard, "RasterizerDiscard" },
				{ ::Core::BaseGraphicsData::ModifiedValue::SampleAlphaToCoverage, "SampleAlphaToCoverage" },
				{ ::Core::BaseGraphicsData::ModifiedValue::SampleAlphaToOne, "SampleAlphaToOne" },
				{ ::Core::BaseGraphicsData::ModifiedValue::SampleCoverage, "SampleCoverage" },
				{ ::Core::BaseGraphicsData::ModifiedValue::SampleMask, "SampleMask" },
				{ ::Core::BaseGraphicsData::ModifiedValue::ScissorTest, "ScissorTest" },
				{ ::Core::BaseGraphicsData::ModifiedValue::StencilTest, "StencilTest" },
				{ ::Core::BaseGraphicsData::ModifiedValue::Multisample, "Multisample" },
				{ ::Core::BaseGraphicsData::ModifiedValue::FramebufferSrgb, "FramebufferSrgb" },
				{ ::Core::BaseGraphicsData::ModifiedValue::ProgramPointSize, "ProgramPointSize" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::BaseGraphicsData::ModifiedValue>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::BaseGraphicsData::ModifiedValue> map = {
				{ "None", ::Core::BaseGraphicsData::ModifiedValue::None },
				{ "Blend", ::Core::BaseGraphicsData::ModifiedValue::Blend },
				{ "CullFace", ::Core::BaseGraphicsData::ModifiedValue::CullFace },
				{ "DepthTest", ::Core::BaseGraphicsData::ModifiedValue::DepthTest },
				{ "Dither", ::Core::BaseGraphicsData::ModifiedValue::Dither },
				{ "PolygonOffsetFill", ::Core::BaseGraphicsData::ModifiedValue::PolygonOffsetFill },
				{ "PolygonOffsetLine", ::Core::BaseGraphicsData::ModifiedValue::PolygonOffsetLine },
				{ "PolygonOffsetPoint", ::Core::BaseGraphicsData::ModifiedValue::PolygonOffsetPoint },
				{ "PrimitiveRestart", ::Core::BaseGraphicsData::ModifiedValue::PrimitiveRestart },
				{ "PrimitiveRestartFixedIndex", ::Core::BaseGraphicsData::ModifiedValue::PrimitiveRestartFixedIndex },
				{ "RasterizerDiscard", ::Core::BaseGraphicsData::ModifiedValue::RasterizerDiscard },
				{ "SampleAlphaToCoverage", ::Core::BaseGraphicsData::ModifiedValue::SampleAlphaToCoverage },
				{ "SampleAlphaToOne", ::Core::BaseGraphicsData::ModifiedValue::SampleAlphaToOne },
				{ "SampleCoverage", ::Core::BaseGraphicsData::ModifiedValue::SampleCoverage },
				{ "SampleMask", ::Core::BaseGraphicsData::ModifiedValue::SampleMask },
				{ "ScissorTest", ::Core::BaseGraphicsData::ModifiedValue::ScissorTest },
				{ "StencilTest", ::Core::BaseGraphicsData::ModifiedValue::StencilTest },
				{ "Multisample", ::Core::BaseGraphicsData::ModifiedValue::Multisample },
				{ "FramebufferSrgb", ::Core::BaseGraphicsData::ModifiedValue::FramebufferSrgb },
				{ "ProgramPointSize", ::Core::BaseGraphicsData::ModifiedValue::ProgramPointSize }
        };

        return map;
    }
}; // struct R<Core::BaseGraphicsData::ModifiedValue>

template<>
struct R<Core::BaseGraphicsData::Modifier>
{
    static constexpr std::string_view Name() { return "Modifier"; }
    static constexpr std::size_t Size() { return 3; }
    static constexpr std::string_view ParentScope() { return "Core::BaseGraphicsData"; }

    static std::string_view ToString(::Core::BaseGraphicsData::Modifier value)
    {
        const auto& data = R<Core::BaseGraphicsData::Modifier>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::BaseGraphicsData::Modifier> FromString(std::string_view value)
    {
        const auto& data = R<Core::BaseGraphicsData::Modifier>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::BaseGraphicsData::Modifier, 3>& ToArrayC()
    {
        static constexpr std::array<::Core::BaseGraphicsData::Modifier, 3> constants = {
				::Core::BaseGraphicsData::Modifier::None,
				::Core::BaseGraphicsData::Modifier::Enable,
				::Core::BaseGraphicsData::Modifier::Disable
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

    static const std::unordered_map<::Core::BaseGraphicsData::Modifier, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::BaseGraphicsData::Modifier, std::string_view> map = {
				{ ::Core::BaseGraphicsData::Modifier::None, "None" },
				{ ::Core::BaseGraphicsData::Modifier::Enable, "Enable" },
				{ ::Core::BaseGraphicsData::Modifier::Disable, "Disable" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::BaseGraphicsData::Modifier>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::BaseGraphicsData::Modifier> map = {
				{ "None", ::Core::BaseGraphicsData::Modifier::None },
				{ "Enable", ::Core::BaseGraphicsData::Modifier::Enable },
				{ "Disable", ::Core::BaseGraphicsData::Modifier::Disable }
        };

        return map;
    }
}; // struct R<Core::BaseGraphicsData::Modifier>

template<>
struct R<Core::BaseGraphicsData>
{
    static constexpr std::string_view Name() { return "BaseGraphicsData"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::vector<ModifierParam>", "_drawModifiers" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::BaseGraphicsData& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize<Core::BaseGraphicsData>(obj);
        }

		s.write("_drawModifiers", obj._drawModifiers);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::BaseGraphicsData>(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::BaseGraphicsData& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::BaseGraphicsData>(obj);
        }

		s.write("_drawModifiers", obj._drawModifiers);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::BaseGraphicsData>(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::BaseGraphicsData& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::BaseGraphicsData>(obj);
        }

		s.read("_drawModifiers", obj._drawModifiers, 4);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::BaseGraphicsData>(obj, s.logs());
        }
    }
}; // struct R<Core::BaseGraphicsData>

template<>
struct R<Core::InterleavedGraphicsData>
{
    static constexpr std::string_view Name() { return "InterleavedGraphicsData"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 0; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::InterleavedGraphicsData& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize<Core::InterleavedGraphicsData>(obj);
        }

		s.write(R<Core::BaseGraphicsData>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::InterleavedGraphicsData>(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::InterleavedGraphicsData& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::InterleavedGraphicsData>(obj);
        }

		s.write(R<Core::BaseGraphicsData>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::InterleavedGraphicsData>(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::InterleavedGraphicsData& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::InterleavedGraphicsData>(obj);
        }

		R<Core::BaseGraphicsData>::Deserialize<RImpl>(s, obj, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::InterleavedGraphicsData>(obj, s.logs());
        }
    }
}; // struct R<Core::InterleavedGraphicsData>

template<>
struct R<Core::SeparTextureGraphicsData>
{
    static constexpr std::string_view Name() { return "SeparTextureGraphicsData"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 0; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::SeparTextureGraphicsData& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize<Core::SeparTextureGraphicsData>(obj);
        }

		s.write(R<Core::BaseGraphicsData>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::SeparTextureGraphicsData>(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::SeparTextureGraphicsData& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::SeparTextureGraphicsData>(obj);
        }

		s.write(R<Core::BaseGraphicsData>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::SeparTextureGraphicsData>(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::SeparTextureGraphicsData& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::SeparTextureGraphicsData>(obj);
        }

		R<Core::BaseGraphicsData>::Deserialize<RImpl>(s, obj, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::SeparTextureGraphicsData>(obj, s.logs());
        }
    }
}; // struct R<Core::SeparTextureGraphicsData>
// clang-format on
