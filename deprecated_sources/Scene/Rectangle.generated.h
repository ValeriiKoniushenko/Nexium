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
struct R<Core::SceneObj::Rectangle>
{
    static constexpr std::string_view Name() { return "Rectangle"; }
    static constexpr std::string_view FullName() { return "Core::SceneObj::Rectangle"; }
    static constexpr std::string_view ParentScope() { return "Core::SceneObj"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "StringAtom", "_textureName", {} },
			{ "StringAtom", "_atlasName", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::SceneObj::Rectangle& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::SceneObj::Rectangle>(obj);
        }

		s.write(R<Core::SceneObject>::Serialize<RImpl>(obj).getData());
		s.write("_textureName", obj._textureName);
		s.write("_atlasName", obj._atlasName);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::SceneObj::Rectangle>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::SceneObj::Rectangle& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::SceneObj::Rectangle& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::SceneObj::Rectangle>(obj);
        }

		R<Core::SceneObject>::Deserialize<RImpl>(s, obj, true);
		s.read("_textureName", obj._textureName, "red.png"_atom, 16);
		s.read("_atlasName", obj._atlasName, "default"_atom, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::SceneObj::Rectangle>(obj, s.logs());
        }
    }

    static bool GetField(Core::SceneObj::Rectangle& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_textureName") == 0)
        {
            onFound((&obj._textureName), "StringAtom");
            return true;
        }
        if (std::strcmp(fieldName, "_atlasName") == 0)
        {
            onFound((&obj._atlasName), "StringAtom");
            return true;
        }

		return false;
    }
}; // struct R<Core::SceneObj::Rectangle>

template<>
struct R<Core::SceneObj::RectangleAnimated>
{
    static constexpr std::string_view Name() { return "RectangleAnimated"; }
    static constexpr std::string_view FullName() { return "Core::SceneObj::RectangleAnimated"; }
    static constexpr std::string_view ParentScope() { return "Core::SceneObj"; }
    static constexpr std::size_t GetFieldNumbers() { return 0; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::SceneObj::RectangleAnimated& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::SceneObj::RectangleAnimated>(obj);
        }

		s.write(R<Core::SceneObj::Rectangle>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::SceneObj::RectangleAnimated>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::SceneObj::RectangleAnimated& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::SceneObj::RectangleAnimated& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::SceneObj::RectangleAnimated>(obj);
        }

		R<Core::SceneObj::Rectangle>::Deserialize<RImpl>(s, obj, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::SceneObj::RectangleAnimated>(obj, s.logs());
        }
    }

    static bool GetField(Core::SceneObj::RectangleAnimated& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;

		return false;
    }
}; // struct R<Core::SceneObj::RectangleAnimated>
// clang-format on
