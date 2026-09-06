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
struct R<Core::TextureAsset>
{
    static constexpr std::string_view Name() { return "TextureAsset"; }
    static constexpr std::string_view FullName() { return "Core::TextureAsset"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::filesystem::path", "_path", {} },
			{ "bool", "_isFlipVertically", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::TextureAsset& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::TextureAsset>(obj);
        }

		s.write(R<Core::BaseAsset>::Serialize<RImpl>(obj).getData());
		s.write("_path", obj._path);
		s.write("_isFlipVertically", obj._isFlipVertically);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::TextureAsset>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::TextureAsset& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::TextureAsset& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::TextureAsset>(obj);
        }

		R<Core::BaseAsset>::Deserialize<RImpl>(s, obj, true);
		s.read("_path", obj._path, decltype(obj._path){}, 16);
		s.read("_isFlipVertically", obj._isFlipVertically, false, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::TextureAsset>(obj, s.logs());
        }
    }

    static bool GetField(Core::TextureAsset& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_path") == 0)
        {
            onFound((&obj._path), "std::filesystem::path");
            return true;
        }
        if (std::strcmp(fieldName, "_isFlipVertically") == 0)
        {
            onFound((&obj._isFlipVertically), "bool");
            return true;
        }

		return false;
    }
}; // struct R<Core::TextureAsset>
// clang-format on
