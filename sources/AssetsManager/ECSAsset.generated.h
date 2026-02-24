/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

#include <array>
#include <optional>
#include <string>
#include <unordered_map>

template<>
struct R<Core::ECSAsset::Status>
{
    static consteval std::string_view Name() { return "Status"; }
    static consteval std::size_t Size() { return 5; }
    static consteval std::string_view ParentScope() { return "Core::ECSAsset"; }

    static std::string_view ToString(::Core::ECSAsset::Status value)
    {
        const auto& data = R<Core::ECSAsset::Status>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::ECSAsset::Status> FromString(std::string_view value)
    {
        const auto& data = R<Core::ECSAsset::Status>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static consteval const std::array<::Core::ECSAsset::Status, 5>& ToArrayC()
    {
        static constexpr std::array<::Core::ECSAsset::Status, 5> constants = {
				::Core::ECSAsset::Status::NotLoaded,
				::Core::ECSAsset::Status::PreLoaded,
				::Core::ECSAsset::Status::Loaded,
				::Core::ECSAsset::Status::PreLoadingError,
				::Core::ECSAsset::Status::LoadingError
        };

        return constants;
    }

    static consteval const std::array<std::string_view, 5>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 5> names = {
				std::string_view("NotLoaded"),
				std::string_view("PreLoaded"),
				std::string_view("Loaded"),
				std::string_view("PreLoadingError"),
				std::string_view("LoadingError")
        };

        return names;
    }

    static const std::unordered_map<::Core::ECSAsset::Status, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::ECSAsset::Status, std::string_view> map = {
				{ ::Core::ECSAsset::Status::NotLoaded, "NotLoaded" },
				{ ::Core::ECSAsset::Status::PreLoaded, "PreLoaded" },
				{ ::Core::ECSAsset::Status::Loaded, "Loaded" },
				{ ::Core::ECSAsset::Status::PreLoadingError, "PreLoadingError" },
				{ ::Core::ECSAsset::Status::LoadingError, "LoadingError" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::ECSAsset::Status>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::ECSAsset::Status> map = {
				{ "NotLoaded", ::Core::ECSAsset::Status::NotLoaded },
				{ "PreLoaded", ::Core::ECSAsset::Status::PreLoaded },
				{ "Loaded", ::Core::ECSAsset::Status::Loaded },
				{ "PreLoadingError", ::Core::ECSAsset::Status::PreLoadingError },
				{ "LoadingError", ::Core::ECSAsset::Status::LoadingError }
        };

        return map;
    }
}; // struct R<Core::ECSAsset::Status>

