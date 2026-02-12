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

    namespace Core::ShaderType
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 4; }

        [[nodiscard]] std::optional<::Core::ShaderType> FromString(const std::string& value);
        [[nodiscard]] const std::string& ToString(::Core::ShaderType value);

        [[nodiscard]] const std::array<::Core::ShaderType, 4>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 4>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::ShaderType, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::ShaderType>& ToMapNC();

    } // namespace Core::ShaderType

    [[nodiscard]] const std::string& ToString(::Core::ShaderType value);

} // namespace
