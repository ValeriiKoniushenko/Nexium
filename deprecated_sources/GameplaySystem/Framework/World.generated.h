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
struct R<Core::LightningProps>
{
    static constexpr std::string_view Name() { return "LightningProps"; }
    static constexpr std::string_view FullName() { return "Core::LightningProps"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 6; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "float", "ambientStrength", {} },
			{ "float", "minLightStrength", {} },
			{ "float", "specularStrength", {} },
			{ "float", "specularPow", {} },
			{ "glm::vec3", "sunDirection", {} },
			{ "Core::NormColor3", "color", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::LightningProps& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::LightningProps>(obj);
        }

		s.write("ambientStrength", obj.ambientStrength);
		s.write("minLightStrength", obj.minLightStrength);
		s.write("specularStrength", obj.specularStrength);
		s.write("specularPow", obj.specularPow);
		s.write("sunDirection", obj.sunDirection);
		s.write("color", obj.color);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::LightningProps>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::LightningProps& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::LightningProps& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::LightningProps>(obj);
        }

		s.read("ambientStrength", obj.ambientStrength, 1.f, 16);
		s.read("minLightStrength", obj.minLightStrength, 0.2f, 16);
		s.read("specularStrength", obj.specularStrength, 0.5f, 16);
		s.read("specularPow", obj.specularPow, 32.f, 16);
		s.read("sunDirection", obj.sunDirection, glm::vec3(0.5f), 16);
		s.read("color", obj.color, Core::NormColor3(1.f), 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::LightningProps>(obj, s.logs());
        }
    }

    static bool GetField(Core::LightningProps& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "ambientStrength") == 0)
        {
            onFound((&obj.ambientStrength), "float");
            return true;
        }
        if (std::strcmp(fieldName, "minLightStrength") == 0)
        {
            onFound((&obj.minLightStrength), "float");
            return true;
        }
        if (std::strcmp(fieldName, "specularStrength") == 0)
        {
            onFound((&obj.specularStrength), "float");
            return true;
        }
        if (std::strcmp(fieldName, "specularPow") == 0)
        {
            onFound((&obj.specularPow), "float");
            return true;
        }
        if (std::strcmp(fieldName, "sunDirection") == 0)
        {
            onFound((&obj.sunDirection), "glm::vec3");
            return true;
        }
        if (std::strcmp(fieldName, "color") == 0)
        {
            onFound((&obj.color), "Core::NormColor3");
            return true;
        }

		return false;
    }
}; // struct R<Core::LightningProps>

template<>
struct R<Core::World>
{
    static constexpr std::string_view Name() { return "World"; }
    static constexpr std::string_view FullName() { return "Core::World"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "StringAtom", "worldName", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::World& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::World>(obj);
        }

		s.write("worldName", obj.worldName);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::World>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::World& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::World& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::World>(obj);
        }

		s.read("worldName", obj.worldName, "Default", 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::World>(obj, s.logs());
        }
    }

    static bool GetField(Core::World& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "worldName") == 0)
        {
            onFound((&obj.worldName), "StringAtom");
            return true;
        }

		return false;
    }
}; // struct R<Core::World>
// clang-format on
