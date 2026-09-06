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
    static constexpr std::string_view FullName() { return "Core::AssetImpl::StaticMeshBundle::AssetData"; }
    static constexpr std::string_view ParentScope() { return "Core::AssetImpl::StaticMeshBundle"; }
    static constexpr std::size_t GetFieldNumbers() { return 5; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::filesystem::path", "meshPath", {} },
			{ "StringAtom", "mainShader", {} },
			{ "StringAtom", "outlineShader", {} },
			{ "float", "onLoadScale", {} },
			{ "std::vector<std::string>", "assimpPostProcess", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
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
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::AssetImpl::StaticMeshBundle::AssetData& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::AssetImpl::StaticMeshBundle::AssetData& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::AssetImpl::StaticMeshBundle::AssetData>(obj);
        }

		s.read("meshPath", obj.meshPath, decltype(obj.meshPath){}, 16);
		s.read("mainShader", obj.mainShader, decltype(obj.mainShader){}, 16);
		s.read("outlineShader", obj.outlineShader, decltype(obj.outlineShader){}, 16);
		s.read("onLoadScale", obj.onLoadScale, 1.0f, 16);
		s.read("assimpPostProcess", obj.assimpPostProcess, decltype(obj.assimpPostProcess){}, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::AssetImpl::StaticMeshBundle::AssetData>(obj, s.logs());
        }
    }

    static bool GetField(Core::AssetImpl::StaticMeshBundle::AssetData& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "meshPath") == 0)
        {
            onFound((&obj.meshPath), "std::filesystem::path");
            return true;
        }
        if (std::strcmp(fieldName, "mainShader") == 0)
        {
            onFound((&obj.mainShader), "StringAtom");
            return true;
        }
        if (std::strcmp(fieldName, "outlineShader") == 0)
        {
            onFound((&obj.outlineShader), "StringAtom");
            return true;
        }
        if (std::strcmp(fieldName, "onLoadScale") == 0)
        {
            onFound((&obj.onLoadScale), "float");
            return true;
        }
        if (std::strcmp(fieldName, "assimpPostProcess") == 0)
        {
            onFound((&obj.assimpPostProcess), "std::vector<std::string>");
            return true;
        }

		return false;
    }
}; // struct R<Core::AssetImpl::StaticMeshBundle::AssetData>
// clang-format on
