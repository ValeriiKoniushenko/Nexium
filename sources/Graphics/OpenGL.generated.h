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

    namespace Core::ViewportMode
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 2; }

        [[nodiscard]] std::optional<::Core::ViewportMode> FromString(const std::string& value);
        [[nodiscard]] const std::string& ToString(::Core::ViewportMode value);

        [[nodiscard]] const std::array<::Core::ViewportMode, 2>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 2>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::ViewportMode, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::ViewportMode>& ToMapNC();

    } // namespace Core::ViewportMode

    [[nodiscard]] const std::string& ToString(::Core::ViewportMode value);

} // namespace
