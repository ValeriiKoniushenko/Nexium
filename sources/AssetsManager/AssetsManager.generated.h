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

    namespace Core::AssetsManager::NodeType
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 5; }

        [[nodiscard]] const std::string& ToString(::Core::AssetsManager::NodeType value);
        [[nodiscard]] std::optional<::Core::AssetsManager::NodeType> FromString(const std::string& value);

        [[nodiscard]] const std::array<::Core::AssetsManager::NodeType, 5>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 5>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::AssetsManager::NodeType, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::AssetsManager::NodeType>& ToMapNC();

    } // namespace Core::AssetsManager::NodeType

} // namespace
