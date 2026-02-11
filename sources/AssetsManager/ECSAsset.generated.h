/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

#pragma once
#include <optional>
#include <string>
#include <array>
#include <unordered_map>

namespace R
{

    namespace Core::ECSAsset::Status
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 5; }

        [[nodiscard]] const std::string& ToString(::Core::ECSAsset::Status value);
        [[nodiscard]] std::optional<::Core::ECSAsset::Status> FromString(const std::string& value);

        [[nodiscard]] const std::array<::Core::ECSAsset::Status, 5>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 5>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::ECSAsset::Status, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::ECSAsset::Status>& ToMapNC();

    } // namespace Core::ECSAsset::Status

} // namespace
