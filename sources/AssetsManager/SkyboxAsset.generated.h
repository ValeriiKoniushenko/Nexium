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

    namespace Core::SkyboxAsset::Direction
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 6; }

        [[nodiscard]] const std::string& ToString(::Core::SkyboxAsset::Direction value);
        [[nodiscard]] std::optional<::Core::SkyboxAsset::Direction> FromString(const std::string& value);

        [[nodiscard]] const std::array<::Core::SkyboxAsset::Direction, 6>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 6>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::SkyboxAsset::Direction, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::SkyboxAsset::Direction>& ToMapNC();

    } // namespace Core::SkyboxAsset::Direction

} // namespace
