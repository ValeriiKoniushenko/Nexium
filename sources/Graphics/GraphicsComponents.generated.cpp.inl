/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

namespace R
{

    namespace Core::GraphicsComponentData::ModifiedValue
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "ModifiedValue"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::GraphicsComponentData"; return name; }

        const std::string& ToString(::Core::GraphicsComponentData::ModifiedValue value)
        {
            const auto& data = R::Core::GraphicsComponentData::ModifiedValue::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static const std::string empty{};
            return empty;
        }

        std::optional<::Core::GraphicsComponentData::ModifiedValue> FromString(const std::string& value)
        {
            const auto& data = R::Core::GraphicsComponentData::ModifiedValue::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::GraphicsComponentData::ModifiedValue, 20>& ToArrayC()
        {
            static const std::array<::Core::GraphicsComponentData::ModifiedValue, 20> constants = {
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
				::Core::GraphicsComponentData::ModifiedValue::ProgramPointSize,
				::Core::GraphicsComponentData::ModifiedValue::None
            };

            return constants;
        }

        const std::array<std::string, 20>& ToArrayN()
        {
            static const std::array<std::string, 20> names = {
				std::string("Blend"),
				std::string("CullFace"),
				std::string("DepthTest"),
				std::string("Dither"),
				std::string("PolygonOffsetFill"),
				std::string("PolygonOffsetLine"),
				std::string("PolygonOffsetPoint"),
				std::string("PrimitiveRestart"),
				std::string("PrimitiveRestartFixedIndex"),
				std::string("RasterizerDiscard"),
				std::string("SampleAlphaToCoverage"),
				std::string("SampleAlphaToOne"),
				std::string("SampleCoverage"),
				std::string("SampleMask"),
				std::string("ScissorTest"),
				std::string("StencilTest"),
				std::string("Multisample"),
				std::string("FramebufferSrgb"),
				std::string("ProgramPointSize"),
				std::string("None")
            };

            return names;
        }

        const std::unordered_map<::Core::GraphicsComponentData::ModifiedValue, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::GraphicsComponentData::ModifiedValue, std::string> map = {
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
				{ ::Core::GraphicsComponentData::ModifiedValue::ProgramPointSize, "ProgramPointSize" },
				{ ::Core::GraphicsComponentData::ModifiedValue::None, "None" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::GraphicsComponentData::ModifiedValue>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::GraphicsComponentData::ModifiedValue> map = {
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
				{ "ProgramPointSize", ::Core::GraphicsComponentData::ModifiedValue::ProgramPointSize },
				{ "None", ::Core::GraphicsComponentData::ModifiedValue::None }
            };

            return map;
        }

    } // namespace Core::GraphicsComponentData::ModifiedValue

    const std::string& ToString(::Core::GraphicsComponentData::ModifiedValue value)
    {
        return Core::GraphicsComponentData::ModifiedValue::ToString(value);
    }

    namespace Core::GraphicsComponentData::Modifier
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "Modifier"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::GraphicsComponentData"; return name; }

        const std::string& ToString(::Core::GraphicsComponentData::Modifier value)
        {
            const auto& data = R::Core::GraphicsComponentData::Modifier::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static const std::string empty{};
            return empty;
        }

        std::optional<::Core::GraphicsComponentData::Modifier> FromString(const std::string& value)
        {
            const auto& data = R::Core::GraphicsComponentData::Modifier::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::GraphicsComponentData::Modifier, 3>& ToArrayC()
        {
            static const std::array<::Core::GraphicsComponentData::Modifier, 3> constants = {
				::Core::GraphicsComponentData::Modifier::None,
				::Core::GraphicsComponentData::Modifier::Enable,
				::Core::GraphicsComponentData::Modifier::Disable
            };

            return constants;
        }

        const std::array<std::string, 3>& ToArrayN()
        {
            static const std::array<std::string, 3> names = {
				std::string("None"),
				std::string("Enable"),
				std::string("Disable")
            };

            return names;
        }

        const std::unordered_map<::Core::GraphicsComponentData::Modifier, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::GraphicsComponentData::Modifier, std::string> map = {
				{ ::Core::GraphicsComponentData::Modifier::None, "None" },
				{ ::Core::GraphicsComponentData::Modifier::Enable, "Enable" },
				{ ::Core::GraphicsComponentData::Modifier::Disable, "Disable" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::GraphicsComponentData::Modifier>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::GraphicsComponentData::Modifier> map = {
				{ "None", ::Core::GraphicsComponentData::Modifier::None },
				{ "Enable", ::Core::GraphicsComponentData::Modifier::Enable },
				{ "Disable", ::Core::GraphicsComponentData::Modifier::Disable }
            };

            return map;
        }

    } // namespace Core::GraphicsComponentData::Modifier

    const std::string& ToString(::Core::GraphicsComponentData::Modifier value)
    {
        return Core::GraphicsComponentData::Modifier::ToString(value);
    }


} // namespace
