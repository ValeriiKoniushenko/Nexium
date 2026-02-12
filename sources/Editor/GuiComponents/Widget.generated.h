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

    namespace Core::Gui::Align
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 7; }

        [[nodiscard]] std::optional<::Core::Gui::Align> FromString(const std::string& value);
        [[nodiscard]] const std::string& ToString(::Core::Gui::Align value);

        [[nodiscard]] const std::array<::Core::Gui::Align, 7>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 7>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::Gui::Align, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::Gui::Align>& ToMapNC();

    } // namespace Core::Gui::Align

    [[nodiscard]] const std::string& ToString(::Core::Gui::Align value);

} // namespace
