/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

namespace R
{

    namespace Core::ECSAsset::Status
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "Status"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::ECSAsset"; return name; }

        const std::string& ToString(::Core::ECSAsset::Status value)
        {
            const auto& data = R::Core::ECSAsset::Status::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static constexpr std::string empty{};
            return empty;
        }

        std::optional<::Core::ECSAsset::Status> FromString(const std::string& value)
        {
            const auto& data = R::Core::ECSAsset::Status::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::ECSAsset::Status, 5>& ToArrayC()
        {
            static const std::array<::Core::ECSAsset::Status, 5> constants = {
				::Core::ECSAsset::Status::NotLoaded,
				::Core::ECSAsset::Status::PreLoaded,
				::Core::ECSAsset::Status::Loaded,
				::Core::ECSAsset::Status::PreLoadingError,
				::Core::ECSAsset::Status::LoadingError
            };

            return constants;
        }

        const std::array<std::string, 5>& ToArrayN()
        {
            static const std::array<std::string, 5> names = {
				std::string("NotLoaded"),
				std::string("PreLoaded"),
				std::string("Loaded"),
				std::string("PreLoadingError"),
				std::string("LoadingError")
            };

            return names;
        }

        const std::unordered_map<::Core::ECSAsset::Status, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::ECSAsset::Status, std::string> map = {
				{ ::Core::ECSAsset::Status::NotLoaded, "NotLoaded" },
				{ ::Core::ECSAsset::Status::PreLoaded, "PreLoaded" },
				{ ::Core::ECSAsset::Status::Loaded, "Loaded" },
				{ ::Core::ECSAsset::Status::PreLoadingError, "PreLoadingError" },
				{ ::Core::ECSAsset::Status::LoadingError, "LoadingError" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::ECSAsset::Status>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::ECSAsset::Status> map = {
				{ "NotLoaded", ::Core::ECSAsset::Status::NotLoaded },
				{ "PreLoaded", ::Core::ECSAsset::Status::PreLoaded },
				{ "Loaded", ::Core::ECSAsset::Status::Loaded },
				{ "PreLoadingError", ::Core::ECSAsset::Status::PreLoadingError },
				{ "LoadingError", ::Core::ECSAsset::Status::LoadingError }
            };

            return map;
        }

    } // namespace Core::ECSAsset::Status

    const std::string& ToString(::Core::ECSAsset::Status value)
    {
        return Core::ECSAsset::Status::ToString(value);
    }


} // namespace
