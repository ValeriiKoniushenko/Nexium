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

    namespace Core::Gizmo::Direction
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 3; }

        [[nodiscard]] const std::string& ToString(::Core::Gizmo::Direction value);
        [[nodiscard]] std::optional<::Core::Gizmo::Direction> FromString(const std::string& value);

        [[nodiscard]] const std::array<::Core::Gizmo::Direction, 3>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 3>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::Gizmo::Direction, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::Gizmo::Direction>& ToMapNC();

    } // namespace Core::Gizmo::Direction

} // namespace
