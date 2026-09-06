/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

// clang-format off
#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

template<>
struct R<Core::CameraType>
{
    static constexpr std::string_view Name() { return "CameraType"; }
    static constexpr std::string_view FullName() { return "Core::CameraType"; }
    static constexpr std::size_t Size() { return 2; }
    static constexpr std::string_view ParentScope() { return "Core"; }

    static std::string_view ToString(::Core::CameraType value)
    {
        const auto& data = R<Core::CameraType>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static constexpr std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::CameraType> FromString(std::string_view value)
    {
        const auto& data = R<Core::CameraType>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(::Core::CameraType value, RResourceStream<RImpl>& s)
    {
        s.getData() = ToString(value);
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static RResourceStream<RImpl> Serialize(::Core::CameraType value)
    {
        RResourceStream<RImpl> s;
        Serialize<RImpl>(value, s);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, ::Core::CameraType& value)
    {
        auto tmp = FromString(s.getData().template get<std::string>());
        if (tmp.has_value())
            value = tmp.value();
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static ::Core::CameraType Deserialize(const RResourceStream<RImpl>& s)
    {
        ::Core::CameraType out;
        Deserialize<RImpl>(s, out);
        return out;
    }

    static constexpr const std::array<::Core::CameraType, 2>& ToArrayC()
    {
        static constexpr std::array<::Core::CameraType, 2> constants = {
				::Core::CameraType::Perspective,
				::Core::CameraType::Orthographic
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 2>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 2> names = {
				std::string_view("Perspective"),
				std::string_view("Orthographic")
        };

        return names;
    }

    static const std::unordered_map<::Core::CameraType, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::CameraType, std::string_view> map = {
				{ ::Core::CameraType::Perspective, "Perspective" },
				{ ::Core::CameraType::Orthographic, "Orthographic" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::CameraType>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::CameraType> map = {
				{ "Perspective", ::Core::CameraType::Perspective },
				{ "Orthographic", ::Core::CameraType::Orthographic }
        };

        return map;
    }
}; // struct R<Core::CameraType>

template<>
struct R<Core::BaseCamera>
{
    static constexpr std::string_view Name() { return "BaseCamera"; }
    static constexpr std::string_view FullName() { return "Core::BaseCamera"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "float", "_far", {} },
			{ "float", "_near", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::BaseCamera& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::BaseCamera>(obj);
        }

		s.write(R<Core::Actor>::Serialize<RImpl>(obj).getData());
		s.write("_far", obj._far);
		s.write("_near", obj._near);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::BaseCamera>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::BaseCamera& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::BaseCamera& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::BaseCamera>(obj);
        }

		R<Core::Actor>::Deserialize<RImpl>(s, obj, true);
		s.read("_far", obj._far, 10'000.f, 16);
		s.read("_near", obj._near, 0.1f, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::BaseCamera>(obj, s.logs());
        }
    }

    static bool GetField(Core::BaseCamera& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_far") == 0)
        {
            onFound((&obj._far), "float");
            return true;
        }
        if (std::strcmp(fieldName, "_near") == 0)
        {
            onFound((&obj._near), "float");
            return true;
        }

		return false;
    }
}; // struct R<Core::BaseCamera>

template<>
struct R<Core::OrthographicCamera>
{
    static constexpr std::string_view Name() { return "OrthographicCamera"; }
    static constexpr std::string_view FullName() { return "Core::OrthographicCamera"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "float", "_zoom", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::OrthographicCamera& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::OrthographicCamera>(obj);
        }

		s.write(R<Core::BaseCamera>::Serialize<RImpl>(obj).getData());
		s.write("_zoom", obj._zoom);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::OrthographicCamera>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::OrthographicCamera& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::OrthographicCamera& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::OrthographicCamera>(obj);
        }

		R<Core::BaseCamera>::Deserialize<RImpl>(s, obj, true);
		s.read("_zoom", obj._zoom, 1.f, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::OrthographicCamera>(obj, s.logs());
        }
    }

    static bool GetField(Core::OrthographicCamera& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_zoom") == 0)
        {
            onFound((&obj._zoom), "float");
            return true;
        }

		return false;
    }
}; // struct R<Core::OrthographicCamera>

template<>
struct R<Core::PerspectiveCamera>
{
    static constexpr std::string_view Name() { return "PerspectiveCamera"; }
    static constexpr std::string_view FullName() { return "Core::PerspectiveCamera"; }
    static constexpr std::string_view ParentScope() { return "Core"; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "Core::FSize2", "_aspect", {} },
			{ "float", "_fov", {} },
		};
    }
    static constexpr std::unordered_map<std::string, RClassField> GetFieldsMap()
    {
        return RInternal::GetClassFieldsAsMap(GetFields());
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Core::PerspectiveCamera& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::PerspectiveCamera>(obj);
        }

		s.write(R<Core::BaseCamera>::Serialize<RImpl>(obj).getData());
		s.write("_aspect", obj._aspect);
		s.write("_fov", obj._fov);
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::PerspectiveCamera>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::PerspectiveCamera& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::PerspectiveCamera& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::PerspectiveCamera>(obj);
        }

		R<Core::BaseCamera>::Deserialize<RImpl>(s, obj, true);
		s.read("_aspect", obj._aspect, Core::FSize2{ 1.f, 1.f }, 16);
		s.read("_fov", obj._fov, 75.f, 16);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::PerspectiveCamera>(obj, s.logs());
        }
    }

    static bool GetField(Core::PerspectiveCamera& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;
        if (std::strcmp(fieldName, "_aspect") == 0)
        {
            onFound((&obj._aspect), "Core::FSize2");
            return true;
        }
        if (std::strcmp(fieldName, "_fov") == 0)
        {
            onFound((&obj._fov), "float");
            return true;
        }

		return false;
    }
}; // struct R<Core::PerspectiveCamera>
// clang-format on
