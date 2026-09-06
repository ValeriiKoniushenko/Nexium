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
struct R<Core::AbstractComponent>
{
    static constexpr std::string_view Name() { return "AbstractComponent"; }
    static constexpr std::string_view FullName() { return "Core::AbstractComponent"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "bool", "_isEnabled", {} },
			{ "bool", "_noTick", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::AbstractComponent& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::AbstractComponent>(obj);
        }

		s.write("_isEnabled", obj._isEnabled);
		s.write("_noTick", obj._noTick);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::AbstractComponent>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::AbstractComponent& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::AbstractComponent& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::AbstractComponent>(obj);
        }

		s.read("_isEnabled", obj._isEnabled, true, 16);
		s.read("_noTick", obj._noTick, false, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::AbstractComponent>(obj, s.logs());
        }
    }

    static bool GetField(Core::AbstractComponent& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_isEnabled") == 0)
        {
            onFound((&obj._isEnabled), "bool");
            return true;
        }
        if (std::strcmp(fieldName, "_noTick") == 0)
        {
            onFound((&obj._noTick), "bool");
            return true;
        }

		return false;
    }
}; // struct R<Core::AbstractComponent>

template<>
struct R<Core::BaseComponent>
{
    static constexpr std::string_view Name() { return "BaseComponent"; }
    static constexpr std::string_view FullName() { return "Core::BaseComponent"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 3; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::vector<Core::IntrusivePtr<Core::BaseComponent>>", "_children", {} },
			{ "Core::StringAtom", "_name", {} },
			{ "Core::StringAtom", "_type", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::BaseComponent& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::BaseComponent>(obj);
        }

		s.write(R<Core::AbstractComponent>::Serialize<RImpl>(obj).getData());
		s.write("_children", obj._children);
		s.write("_name", obj._name);
		s.write("_type", obj._type);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::BaseComponent>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::BaseComponent& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::BaseComponent& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::BaseComponent>(obj);
        }

		R<Core::AbstractComponent>::Deserialize<RImpl>(s, obj, true);
		s.read("_children", obj._children, decltype(obj._children){}, 16);
		s.read("_name", obj._name, decltype(obj._name){}, 16);
		s.read("_type", obj._type, decltype(obj._type){}, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::BaseComponent>(obj, s.logs());
        }
    }

    static bool GetField(Core::BaseComponent& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_children") == 0)
        {
            onFound((&obj._children), "std::vector<Core::IntrusivePtr<Core::BaseComponent>>");
            return true;
        }
        if (std::strcmp(fieldName, "_name") == 0)
        {
            onFound((&obj._name), "Core::StringAtom");
            return true;
        }
        if (std::strcmp(fieldName, "_type") == 0)
        {
            onFound((&obj._type), "Core::StringAtom");
            return true;
        }

		return false;
    }
}; // struct R<Core::BaseComponent>

template<>
struct R<Core::InvalidComponent>
{
    static constexpr std::string_view Name() { return "InvalidComponent"; }
    static constexpr std::string_view FullName() { return "Core::InvalidComponent"; }
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
    static void Serialize(const Core::InvalidComponent& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::InvalidComponent>(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::InvalidComponent>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::InvalidComponent& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::InvalidComponent& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::InvalidComponent>(obj);
        }

		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::InvalidComponent>(obj, s.logs());
        }
    }

    static bool GetField(Core::InvalidComponent& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;

		return false;
    }
}; // struct R<Core::InvalidComponent>
// clang-format on
