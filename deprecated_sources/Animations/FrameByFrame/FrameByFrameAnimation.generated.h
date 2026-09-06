/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

// clang-format off
#include <string>
#include <string_view>
#include <vector>

template<>
struct R<Core::Animation::Frame>
{
    static constexpr std::string_view Name() { return "Frame"; }
    static constexpr std::string_view FullName() { return "Core::Animation::Frame"; }
    static constexpr std::string_view ParentScope() { return "Core::Animation"; }
    static constexpr std::size_t GetFieldNumbers() { return 4; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "Core::StringAtom", "name", {} },
			{ "std::optional<Core::StringAtom>", "textureName", {} },
			{ "Core::GlobalPosition2F", "uvOffset", {} },
			{ "Core::GlobalPosition2F", "uvSize", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Animation::Frame& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Animation::Frame>(obj);
        }

		s.write("name", obj.name);
		s.write("textureName", obj.textureName);
		s.write("uvOffset", obj.uvOffset);
		s.write("uvSize", obj.uvSize);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Animation::Frame>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Animation::Frame& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Animation::Frame& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Animation::Frame>(obj);
        }

		s.read("name", obj.name, decltype(obj.name){}, 16);
		s.read("textureName", obj.textureName, decltype(obj.textureName){}, 16);
		s.read("uvOffset", obj.uvOffset, Core::GlobalPosition2F{ 0.f, 0.f }, 16);
		s.read("uvSize", obj.uvSize, Core::GlobalPosition2F{ 1.f, 1.f }, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Animation::Frame>(obj, s.logs());
        }
    }

    static bool GetField(Core::Animation::Frame& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "name") == 0)
        {
            onFound((&obj.name), "Core::StringAtom");
            return true;
        }
        if (std::strcmp(fieldName, "textureName") == 0)
        {
            onFound((&obj.textureName), "std::optional<Core::StringAtom>");
            return true;
        }
        if (std::strcmp(fieldName, "uvOffset") == 0)
        {
            onFound((&obj.uvOffset), "Core::GlobalPosition2F");
            return true;
        }
        if (std::strcmp(fieldName, "uvSize") == 0)
        {
            onFound((&obj.uvSize), "Core::GlobalPosition2F");
            return true;
        }

		return false;
    }
}; // struct R<Core::Animation::Frame>

template<>
struct R<Core::Animation::FrameByFrameAnimation>
{
    static constexpr std::string_view Name() { return "FrameByFrameAnimation"; }
    static constexpr std::string_view FullName() { return "Core::Animation::FrameByFrameAnimation"; }
    static constexpr std::string_view ParentScope() { return "Core::Animation"; }
    static constexpr std::size_t GetFieldNumbers() { return 6; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "Core::StringAtom", "_atlasName", {} },
			{ "Core::StringAtom", "_textureName", {} },
			{ "std::vector<Core::Animation::Frame>", "_frames", {} },
			{ "std::size_t", "_currentFrame", {} },
			{ "float", "_frameTimeAccumulator", {} },
			{ "float", "_fps", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Animation::FrameByFrameAnimation& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Animation::FrameByFrameAnimation>(obj);
        }

		s.write(R<Core::Animation::BaseAnimation>::Serialize<RImpl>(obj).getData());
		s.write("_atlasName", obj._atlasName);
		s.write("_textureName", obj._textureName);
		s.write("_frames", obj._frames);
		s.write("_currentFrame", obj._currentFrame);
		s.write("_frameTimeAccumulator", obj._frameTimeAccumulator);
		s.write("_fps", obj._fps);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Animation::FrameByFrameAnimation>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Animation::FrameByFrameAnimation& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Animation::FrameByFrameAnimation& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Animation::FrameByFrameAnimation>(obj);
        }

		R<Core::Animation::BaseAnimation>::Deserialize<RImpl>(s, obj, true);
		s.read("_atlasName", obj._atlasName, decltype(obj._atlasName){}, 16);
		s.read("_textureName", obj._textureName, decltype(obj._textureName){}, 16);
		s.read("_frames", obj._frames, decltype(obj._frames){}, 16);
		s.read("_currentFrame", obj._currentFrame, 0, 16);
		s.read("_frameTimeAccumulator", obj._frameTimeAccumulator, 0.f, 16);
		s.read("_fps", obj._fps, 10.f, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Animation::FrameByFrameAnimation>(obj, s.logs());
        }
    }

    static bool GetField(Core::Animation::FrameByFrameAnimation& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_atlasName") == 0)
        {
            onFound((&obj._atlasName), "Core::StringAtom");
            return true;
        }
        if (std::strcmp(fieldName, "_textureName") == 0)
        {
            onFound((&obj._textureName), "Core::StringAtom");
            return true;
        }
        if (std::strcmp(fieldName, "_frames") == 0)
        {
            onFound((&obj._frames), "std::vector<Core::Animation::Frame>");
            return true;
        }
        if (std::strcmp(fieldName, "_currentFrame") == 0)
        {
            onFound((&obj._currentFrame), "std::size_t");
            return true;
        }
        if (std::strcmp(fieldName, "_frameTimeAccumulator") == 0)
        {
            onFound((&obj._frameTimeAccumulator), "float");
            return true;
        }
        if (std::strcmp(fieldName, "_fps") == 0)
        {
            onFound((&obj._fps), "float");
            return true;
        }

		return false;
    }
}; // struct R<Core::Animation::FrameByFrameAnimation>
// clang-format on
