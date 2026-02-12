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

    namespace Core::Mouse::Key
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 13; }

        [[nodiscard]] std::optional<::Core::Mouse::Key> FromString(const std::string& value);
        [[nodiscard]] const std::string& ToString(::Core::Mouse::Key value);

        [[nodiscard]] const std::array<::Core::Mouse::Key, 13>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 13>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::Mouse::Key, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::Mouse::Key>& ToMapNC();

    } // namespace Core::Mouse::Key

    [[nodiscard]] const std::string& ToString(::Core::Mouse::Key value);

    namespace Core::Mouse::Mod
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 7; }

        [[nodiscard]] std::optional<::Core::Mouse::Mod> FromString(const std::string& value);
        [[nodiscard]] const std::string& ToString(::Core::Mouse::Mod value);

        [[nodiscard]] const std::array<::Core::Mouse::Mod, 7>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 7>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::Mouse::Mod, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::Mouse::Mod>& ToMapNC();

    } // namespace Core::Mouse::Mod

    [[nodiscard]] const std::string& ToString(::Core::Mouse::Mod value);

} // namespace
