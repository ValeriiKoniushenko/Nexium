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
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 3; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::optional<Core::Color4>", "_textColor" },
			{ "std::optional<Core::Color4>", "_borderColor" },
			{ "glm::vec2", "_size" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::BaseInput& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::Gui::Widget>::Serialize<RImpl>(obj).getData());
		s.write("_textColor", obj._textColor);
		s.write("_borderColor", obj._borderColor);
		s.write("_size", obj._size);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::BaseInput& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::Gui::Widget>::Serialize<RImpl>(obj).getData());
		s.write("_textColor", obj._textColor);
		s.write("_borderColor", obj._borderColor);
		s.write("_size", obj._size);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::BaseInput& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Core::Gui::Widget>::Deserialize<RImpl>(s, obj, true);
		s.read("_textColor", obj._textColor, 4);
		s.read("_borderColor", obj._borderColor, 4);
		s.read("_size", obj._size, glm::vec2{});
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Core::Gui::BaseInput>

template<>
struct R<Core::Gui::TextInput>
{
    static constexpr std::string_view Name() { return "TextInput"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 3; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::string", "_buffer" },
			{ "Core::StringAtom", "_placeholder" },
			{ "int", "_flags" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::TextInput& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::Gui::BaseInput>::Serialize<RImpl>(obj).getData());
		s.write("_buffer", obj._buffer);
		s.write("_placeholder", obj._placeholder);
		s.write("_flags", obj._flags);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::TextInput& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::Gui::BaseInput>::Serialize<RImpl>(obj).getData());
		s.write("_buffer", obj._buffer);
		s.write("_placeholder", obj._placeholder);
		s.write("_flags", obj._flags);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::TextInput& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Core::Gui::BaseInput>::Deserialize<RImpl>(s, obj, true);
		s.read("_buffer", obj._buffer, 4);
		s.read("_placeholder", obj._placeholder, 4);
		s.read("_flags", obj._flags, ImGuiInputTextFlags_None);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Core::Gui::TextInput>

template<>
struct R<Core::Gui::Color3Input>
{
    static constexpr std::string_view Name() { return "Color3Input"; }
    static constexpr std::string_view ParentScope() { return "Core::Gui"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "Core::NormColor3", "_buffer" },
			{ "Core::StringAtom", "_stringBuffer" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gui::Color3Input& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::Gui::BaseInput>::Serialize<RImpl>(obj).getData());
		s.write("_buffer", obj._buffer);
		s.write("_stringBuffer", obj._stringBuffer);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::Gui::Color3Input& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::Gui::BaseInput>::Serialize<RImpl>(obj).getData());
		s.write("_buffer", obj._buffer);
		s.write("_stringBuffer", obj._stringBuffer);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gui::Color3Input& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Core::Gui::BaseInput>::Deserialize<RImpl>(s, obj, true);
		s.read("_buffer", obj._buffer, Core::NormColor3(0.f, 0.f, 0.f));
		s.read("_stringBuffer", obj._stringBuffer, "0 0 0");
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Core::Gui::Color3Input>
// clang-format on
