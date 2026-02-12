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

    namespace Core::GraphicsComponentData::Modifier
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 3; }

        [[nodiscard]] std::optional<::Core::GraphicsComponentData::Modifier> FromString(const std::string& value);
        [[nodiscard]] const std::string& ToString(::Core::GraphicsComponentData::Modifier value);

        [[nodiscard]] const std::array<::Core::GraphicsComponentData::Modifier, 3>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 3>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::GraphicsComponentData::Modifier, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::GraphicsComponentData::Modifier>& ToMapNC();

    } // namespace Core::GraphicsComponentData::Modifier

    [[nodiscard]] const std::string& ToString(::Core::GraphicsComponentData::Modifier value);

} // namespace
