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
struct R<Core::AssetImpl::StaticMeshBundle::AssetData>
{
    static constexpr std::string_view Name() { return "AssetData"; }
    static constexpr std::string_view ParentScope() { return "Core::AssetImpl::StaticMeshBundle"; }
    static constexpr std::size_t GetFieldNumbers() { return 5; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::filesystem::path", "meshPath" },
			{ "StringAtom", "mainShader" },
			{ "StringAtom", "outlineShader" },
			{ "float", "onLoadScale" },
			{ "std::vector<std::string>", "assimpPostProcess" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::AssetImpl::StaticMeshBundle::AssetData& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize<Core::AssetImpl::StaticMeshBundle::AssetData>(obj);
        }

		s.write("meshPath", obj.meshPath);
		s.write("mainShader", obj.mainShader);
		s.write("outlineShader", obj.outlineShader);
		s.write("onLoadScale", obj.onLoadScale);
		s.write("assimpPostProcess", obj.assimpPostProcess);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::AssetImpl::StaticMeshBundle::AssetData>(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::AssetImpl::StaticMeshBundle::AssetData& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::AssetImpl::StaticMeshBundle::AssetData>(obj);
        }

		s.write("meshPath", obj.meshPath);
		s.write("mainShader", obj.mainShader);
		s.write("outlineShader", obj.outlineShader);
		s.write("onLoadScale", obj.onLoadScale);
		s.write("assimpPostProcess", obj.assimpPostProcess);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::AssetImpl::StaticMeshBundle::AssetData>(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::AssetImpl::StaticMeshBundle::AssetData& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::AssetImpl::StaticMeshBundle::AssetData>(obj);
        }

		s.read("meshPath", obj.meshPath, 4);
		s.read("mainShader", obj.mainShader, 4);
		s.read("outlineShader", obj.outlineShader, 4);
		s.read("onLoadScale", obj.onLoadScale, 1.0f);
		s.read("assimpPostProcess", obj.assimpPostProcess, 4);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::AssetImpl::StaticMeshBundle::AssetData>(obj, s.logs());
        }
    }
}; // struct R<Core::AssetImpl::StaticMeshBundle::AssetData>
// clang-format on
