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
struct R<Core::BaseEWC>
{
    static constexpr std::string_view Name() { return "BaseEWC"; }
    static constexpr std::string_view FullName() { return "Core::BaseEWC"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
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
    static void Serialize(const Core::BaseEWC& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::BaseEWC>(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::BaseEWC>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::BaseEWC& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::BaseEWC& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::BaseEWC>(obj);
        }

		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::BaseEWC>(obj, s.logs());
        }
    }

    static bool GetField(Core::BaseEWC& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;

		return false;
    }
}; // struct R<Core::BaseEWC>

template<>
struct R<Core::BaseFloatEWC>
{
    static constexpr std::string_view Name() { return "BaseFloatEWC"; }
    static constexpr std::string_view FullName() { return "Core::BaseFloatEWC"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 7; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "Core::FSize2", "_minWindowSize", {} },
			{ "Core::FSize2", "_size", {} },
			{ "Core::FSize2", "_innerSize", {} },
			{ "Core::FSize2", "_oldSize", {} },
			{ "glm::vec2", "_position", {} },
			{ "glm::vec2", "_innerPosition", {} },
			{ "bool", "_isFitContent", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::BaseFloatEWC& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::BaseFloatEWC>(obj);
        }

		s.write(R<Core::BaseEWC>::Serialize<RImpl>(obj).getData());
		s.write("_minWindowSize", obj._minWindowSize);
		s.write("_size", obj._size);
		s.write("_innerSize", obj._innerSize);
		s.write("_oldSize", obj._oldSize);
		s.write("_position", obj._position);
		s.write("_innerPosition", obj._innerPosition);
		s.write("_isFitContent", obj._isFitContent);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::BaseFloatEWC>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::BaseFloatEWC& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::BaseFloatEWC& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::BaseFloatEWC>(obj);
        }

		R<Core::BaseEWC>::Deserialize<RImpl>(s, obj, true);
		s.read("_minWindowSize", obj._minWindowSize, Core::FSize2(200.f, 200.f), 16);
		s.read("_size", obj._size, decltype(obj._size){}, 16);
		s.read("_innerSize", obj._innerSize, decltype(obj._innerSize){}, 16);
		s.read("_oldSize", obj._oldSize, Core::FSize2{ -1, -1 }, 16);
		s.read("_position", obj._position, glm::vec2{}, 16);
		s.read("_innerPosition", obj._innerPosition, glm::vec2{}, 16);
		s.read("_isFitContent", obj._isFitContent, false, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::BaseFloatEWC>(obj, s.logs());
        }
    }

    static bool GetField(Core::BaseFloatEWC& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_minWindowSize") == 0)
        {
            onFound((&obj._minWindowSize), "Core::FSize2");
            return true;
        }
        if (std::strcmp(fieldName, "_size") == 0)
        {
            onFound((&obj._size), "Core::FSize2");
            return true;
        }
        if (std::strcmp(fieldName, "_innerSize") == 0)
        {
            onFound((&obj._innerSize), "Core::FSize2");
            return true;
        }
        if (std::strcmp(fieldName, "_oldSize") == 0)
        {
            onFound((&obj._oldSize), "Core::FSize2");
            return true;
        }
        if (std::strcmp(fieldName, "_position") == 0)
        {
            onFound((&obj._position), "glm::vec2");
            return true;
        }
        if (std::strcmp(fieldName, "_innerPosition") == 0)
        {
            onFound((&obj._innerPosition), "glm::vec2");
            return true;
        }
        if (std::strcmp(fieldName, "_isFitContent") == 0)
        {
            onFound((&obj._isFitContent), "bool");
            return true;
        }

		return false;
    }
}; // struct R<Core::BaseFloatEWC>

template<>
struct R<Core::BaseMenuBarEWC>
{
    static constexpr std::string_view Name() { return "BaseMenuBarEWC"; }
    static constexpr std::string_view FullName() { return "Core::BaseMenuBarEWC"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
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
    static void Serialize(const Core::BaseMenuBarEWC& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::BaseMenuBarEWC>(obj);
        }

		s.write(R<Core::BaseEWC>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::BaseMenuBarEWC>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::BaseMenuBarEWC& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::BaseMenuBarEWC& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::BaseMenuBarEWC>(obj);
        }

		R<Core::BaseEWC>::Deserialize<RImpl>(s, obj, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::BaseMenuBarEWC>(obj, s.logs());
        }
    }

    static bool GetField(Core::BaseMenuBarEWC& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;

		return false;
    }
}; // struct R<Core::BaseMenuBarEWC>
// clang-format on
