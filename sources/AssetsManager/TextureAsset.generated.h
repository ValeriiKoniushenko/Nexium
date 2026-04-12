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
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::filesystem::path", "_path" },
			{ "bool", "_isFlipVertically" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::TextureAsset& obj)
    {
        RResourceStream<RImpl> s;
		s.write("_path", obj._path);
		s.write("_isFlipVertically", obj._isFlipVertically);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::TextureAsset& obj, RResourceStream<RImpl>& s)
    {
        
		s.write("_path", obj._path);
		s.write("_isFlipVertically", obj._isFlipVertically);
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::TextureAsset& obj)
    {
		s.read("_path", obj._path);
		s.read("_isFlipVertically", obj._isFlipVertically, false);
    }
}; // struct R<Core::TextureAsset>
// clang-format on
