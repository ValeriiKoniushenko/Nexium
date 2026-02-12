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

    namespace Core::Keyboard::KeyState
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 4; }

        [[nodiscard]] std::optional<::Core::Keyboard::KeyState> FromString(const std::string& value);
        [[nodiscard]] const std::string& ToString(::Core::Keyboard::KeyState value);

        [[nodiscard]] const std::array<::Core::Keyboard::KeyState, 4>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 4>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::Keyboard::KeyState, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::Keyboard::KeyState>& ToMapNC();

    } // namespace Core::Keyboard::KeyState

    [[nodiscard]] const std::string& ToString(::Core::Keyboard::KeyState value);

    namespace Core::Keyboard::Key
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 122; }

        [[nodiscard]] std::optional<::Core::Keyboard::Key> FromString(const std::string& value);
        [[nodiscard]] const std::string& ToString(::Core::Keyboard::Key value);

        [[nodiscard]] const std::array<::Core::Keyboard::Key, 122>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 122>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::Keyboard::Key, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::Keyboard::Key>& ToMapNC();

    } // namespace Core::Keyboard::Key

    [[nodiscard]] const std::string& ToString(::Core::Keyboard::Key value);

} // namespace
