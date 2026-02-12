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

    namespace Core::Image::Channel
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 5; }

        [[nodiscard]] std::optional<::Core::Image::Channel> FromString(const std::string& value);
        [[nodiscard]] const std::string& ToString(::Core::Image::Channel value);

        [[nodiscard]] const std::array<::Core::Image::Channel, 5>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 5>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::Image::Channel, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::Image::Channel>& ToMapNC();

    } // namespace Core::Image::Channel

    [[nodiscard]] const std::string& ToString(::Core::Image::Channel value);

} // namespace
