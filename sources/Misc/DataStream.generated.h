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

    namespace Core::DataStream::Result
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 5; }

        [[nodiscard]] std::optional<::Core::DataStream::Result> FromString(const std::string& value);
        [[nodiscard]] const std::string& ToString(::Core::DataStream::Result value);

        [[nodiscard]] const std::array<::Core::DataStream::Result, 5>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 5>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::DataStream::Result, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::DataStream::Result>& ToMapNC();

    } // namespace Core::DataStream::Result

    [[nodiscard]] const std::string& ToString(::Core::DataStream::Result value);

    namespace Core::DataStream::Mode
    {

        // =================== DECLARATIONS =====================
        [[nodiscard]] const std::string& Name();
        [[nodiscard]] const std::string& ParentScope();
        [[nodiscard]] constexpr std::size_t Size() noexcept { return 2; }

        [[nodiscard]] std::optional<::Core::DataStream::Mode> FromString(const std::string& value);
        [[nodiscard]] const std::string& ToString(::Core::DataStream::Mode value);

        [[nodiscard]] const std::array<::Core::DataStream::Mode, 2>& ToArrayC();
        [[nodiscard]] const std::array<std::string, 2>& ToArrayN();
        [[nodiscard]] const std::unordered_map<::Core::DataStream::Mode, std::string>& ToMapCN();
        [[nodiscard]] const std::unordered_map<std::string, ::Core::DataStream::Mode>& ToMapNC();

    } // namespace Core::DataStream::Mode

    [[nodiscard]] const std::string& ToString(::Core::DataStream::Mode value);

} // namespace
