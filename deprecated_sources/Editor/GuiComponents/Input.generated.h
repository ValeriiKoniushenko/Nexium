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
struct R<Core::Gui::BaseInput>
{
    static constexpr std::string_view Name() { return "BaseInput"; }
    static constexpr std::string_view FullName() { return "Core::Gui::BaseInput"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 3; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::optional<Core::Color4>", "_textColor", {} },
			{ "std::optional<Core::Color4>", "_borderColor", {} },
			{ "glm::vec2", "_size", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::BaseInput& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::BaseInput>(obj);
        }

		s.write(R<Core::Gui::Widget>::Serialize<RImpl>(obj).getData());
		s.write("_textColor", obj._textColor);
		s.write("_borderColor", obj._borderColor);
		s.write("_size", obj._size);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::BaseInput>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::BaseInput& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::BaseInput& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gui::BaseInput>(obj);
        }

		R<Core::Gui::Widget>::Deserialize<RImpl>(s, obj, true);
		s.read("_textColor", obj._textColor, decltype(obj._textColor){}, 16);
		s.read("_borderColor", obj._borderColor, decltype(obj._borderColor){}, 16);
		s.read("_size", obj._size, glm::vec2{}, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gui::BaseInput>(obj, s.logs());
        }
    }

    static bool GetField(Core::Gui::BaseInput& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_textColor") == 0)
        {
            onFound((&obj._textColor), "std::optional<Core::Color4>");
            return true;
        }
        if (std::strcmp(fieldName, "_borderColor") == 0)
        {
            onFound((&obj._borderColor), "std::optional<Core::Color4>");
            return true;
        }
        if (std::strcmp(fieldName, "_size") == 0)
        {
            onFound((&obj._size), "glm::vec2");
            return true;
        }

		return false;
    }
}; // struct R<Core::Gui::BaseInput>

template<>
struct R<Core::Gui::TextInput>
{
    static constexpr std::string_view Name() { return "TextInput"; }
    static constexpr std::string_view FullName() { return "Core::Gui::TextInput"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 5; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::string", "_buffer", {} },
			{ "Core::StringAtom", "_placeholder", {} },
			{ "int", "_flags", {} },
			{ "bool", "_needFocus", {} },
			{ "bool", "_needSelectAll", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::TextInput& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::TextInput>(obj);
        }

		s.write(R<Core::Gui::BaseInput>::Serialize<RImpl>(obj).getData());
		s.write("_buffer", obj._buffer);
		s.write("_placeholder", obj._placeholder);
		s.write("_flags", obj._flags);
		s.write("_needFocus", obj._needFocus);
		s.write("_needSelectAll", obj._needSelectAll);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::TextInput>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::TextInput& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::TextInput& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gui::TextInput>(obj);
        }

		R<Core::Gui::BaseInput>::Deserialize<RImpl>(s, obj, true);
		s.read("_buffer", obj._buffer, decltype(obj._buffer){}, 16);
		s.read("_placeholder", obj._placeholder, decltype(obj._placeholder){}, 16);
		s.read("_flags", obj._flags, ImGuiInputTextFlags_None, 16);
		s.read("_needFocus", obj._needFocus, false, 16);
		s.read("_needSelectAll", obj._needSelectAll, false, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gui::TextInput>(obj, s.logs());
        }
    }

    static bool GetField(Core::Gui::TextInput& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_buffer") == 0)
        {
            onFound((&obj._buffer), "std::string");
            return true;
        }
        if (std::strcmp(fieldName, "_placeholder") == 0)
        {
            onFound((&obj._placeholder), "Core::StringAtom");
            return true;
        }
        if (std::strcmp(fieldName, "_flags") == 0)
        {
            onFound((&obj._flags), "int");
            return true;
        }
        if (std::strcmp(fieldName, "_needFocus") == 0)
        {
            onFound((&obj._needFocus), "bool");
            return true;
        }
        if (std::strcmp(fieldName, "_needSelectAll") == 0)
        {
            onFound((&obj._needSelectAll), "bool");
            return true;
        }

		return false;
    }
}; // struct R<Core::Gui::TextInput>

template<>
struct R<Core::Gui::Color3Input>
{
    static constexpr std::string_view Name() { return "Color3Input"; }
    static constexpr std::string_view FullName() { return "Core::Gui::Color3Input"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "Core::NormColor3", "_buffer", {} },
			{ "Core::StringAtom", "_stringBuffer", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::Color3Input& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gui::Color3Input>(obj);
        }

		s.write(R<Core::Gui::BaseInput>::Serialize<RImpl>(obj).getData());
		s.write("_buffer", obj._buffer);
		s.write("_stringBuffer", obj._stringBuffer);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gui::Color3Input>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::Color3Input& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::Color3Input& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gui::Color3Input>(obj);
        }

		R<Core::Gui::BaseInput>::Deserialize<RImpl>(s, obj, true);
		s.read("_buffer", obj._buffer, Core::NormColor3(0.f, 0.f, 0.f), 16);
		s.read("_stringBuffer", obj._stringBuffer, "0 0 0", 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gui::Color3Input>(obj, s.logs());
        }
    }

    static bool GetField(Core::Gui::Color3Input& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_buffer") == 0)
        {
            onFound((&obj._buffer), "Core::NormColor3");
            return true;
        }
        if (std::strcmp(fieldName, "_stringBuffer") == 0)
        {
            onFound((&obj._stringBuffer), "Core::StringAtom");
            return true;
        }

		return false;
    }
}; // struct R<Core::Gui::Color3Input>
// clang-format on
