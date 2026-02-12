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

    namespace Core::Mouse::State
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 3; }

        [[nodiscard]] std::optional<::Core::Mouse::State> FromString(const std::string& value);
        [[nodiscard]] const std::string& ToString(::Core::Mouse::State value);

        [[nodiscard]] const std::array<::Core::Mouse::State, 3>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 3>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::Mouse::State, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::Mouse::State>& ToMapNC();

    } // namespace Core::Mouse::State

    [[nodiscard]] const std::string& ToString(::Core::Mouse::State value);

} // namespace
