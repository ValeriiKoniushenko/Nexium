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
struct R<Core::Gizmo::Direction>
{
    static constexpr std::string_view Name() { return "Direction"; }
    static constexpr std::string_view FullName() { return "Core::Gizmo::Direction"; }
    static constexpr std::size_t Size() { return 3; }
    static constexpr std::string_view ParentScope() { return "Core::Gizmo"; }

    static std::string_view ToString(::Core::Gizmo::Direction value)
    {
        const auto& data = R<Core::Gizmo::Direction>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static constexpr std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::Gizmo::Direction> FromString(std::string_view value)
    {
        const auto& data = R<Core::Gizmo::Direction>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(::Core::Gizmo::Direction value, RResourceStream<RImpl>& s)
    {
        s.getData() = ToString(value);
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static RResourceStream<RImpl> Serialize(::Core::Gizmo::Direction value)
    {
        RResourceStream<RImpl> s;
        Serialize<RImpl>(value, s);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, ::Core::Gizmo::Direction& value)
    {
        auto tmp = FromString(s.getData().template get<std::string>());
        if (tmp.has_value())
            value = tmp.value();
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static ::Core::Gizmo::Direction Deserialize(const RResourceStream<RImpl>& s)
    {
        ::Core::Gizmo::Direction out;
        Deserialize<RImpl>(s, out);
        return out;
    }

    static constexpr const std::array<::Core::Gizmo::Direction, 3>& ToArrayC()
    {
        static constexpr std::array<::Core::Gizmo::Direction, 3> constants = {
				::Core::Gizmo::Direction::X,
				::Core::Gizmo::Direction::Y,
				::Core::Gizmo::Direction::Z
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 3>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 3> names = {
				std::string_view("X"),
				std::string_view("Y"),
				std::string_view("Z")
        };

        return names;
    }

    static const std::unordered_map<::Core::Gizmo::Direction, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::Gizmo::Direction, std::string_view> map = {
				{ ::Core::Gizmo::Direction::X, "X" },
				{ ::Core::Gizmo::Direction::Y, "Y" },
				{ ::Core::Gizmo::Direction::Z, "Z" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::Gizmo::Direction>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::Gizmo::Direction> map = {
				{ "X", ::Core::Gizmo::Direction::X },
				{ "Y", ::Core::Gizmo::Direction::Y },
				{ "Z", ::Core::Gizmo::Direction::Z }
        };

        return map;
    }
}; // struct R<Core::Gizmo::Direction>

template<>
struct R<Core::Gizmo>
{
    static constexpr std::string_view Name() { return "Gizmo"; }
    static constexpr std::string_view FullName() { return "Core::Gizmo"; }
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
    static void Serialize(const Core::Gizmo& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize<Core::Gizmo>(obj);
        }

		s.write(R<Core::StaticMeshBundle>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize<Core::Gizmo>(obj, s.logs());
        }
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Core::Gizmo& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        Serialize(obj, s, noSignals);
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Core::Gizmo& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize<Core::Gizmo>(obj);
        }

		R<Core::StaticMeshBundle>::Deserialize<RImpl>(s, obj, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize<Core::Gizmo>(obj, s.logs());
        }
    }

    static bool GetField(Core::Gizmo& obj, const char* fieldName, const std::function<void(void*, const char*)>& onFound)
    {
        if (!onFound) return false;

		return false;
    }
}; // struct R<Core::Gizmo>
// clang-format on
