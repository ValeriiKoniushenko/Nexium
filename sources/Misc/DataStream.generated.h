/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

// clang-format off#include <array>
#include <optional>
#include <string>
#include <unordered_map>

template<>
struct R<Core::DataStream::Result>
{
    static constexpr std::string_view Name() { return "Result"; }
    static constexpr std::size_t Size() { return 5; }
    static constexpr std::string_view ParentScope() { return "Core::DataStream"; }

    static std::string_view ToString(::Core::DataStream::Result value)
    {
        const auto& data = R<Core::DataStream::Result>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::DataStream::Result> FromString(std::string_view value)
    {
        const auto& data = R<Core::DataStream::Result>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::DataStream::Result, 5>& ToArrayC()
    {
        static constexpr std::array<::Core::DataStream::Result, 5> constants = {
				::Core::DataStream::Result::Success,
				::Core::DataStream::Result::ReadFailed,
				::Core::DataStream::Result::WriteFailed,
				::Core::DataStream::Result::InvalidPassedData,
				::Core::DataStream::Result::CustomProcessingError
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 5>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 5> names = {
				std::string_view("Success"),
				std::string_view("ReadFailed"),
				std::string_view("WriteFailed"),
				std::string_view("InvalidPassedData"),
				std::string_view("CustomProcessingError")
        };

        return names;
    }

    static const std::unordered_map<::Core::DataStream::Result, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::DataStream::Result, std::string_view> map = {
				{ ::Core::DataStream::Result::Success, "Success" },
				{ ::Core::DataStream::Result::ReadFailed, "ReadFailed" },
				{ ::Core::DataStream::Result::WriteFailed, "WriteFailed" },
				{ ::Core::DataStream::Result::InvalidPassedData, "InvalidPassedData" },
				{ ::Core::DataStream::Result::CustomProcessingError, "CustomProcessingError" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::DataStream::Result>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::DataStream::Result> map = {
				{ "Success", ::Core::DataStream::Result::Success },
				{ "ReadFailed", ::Core::DataStream::Result::ReadFailed },
				{ "WriteFailed", ::Core::DataStream::Result::WriteFailed },
				{ "InvalidPassedData", ::Core::DataStream::Result::InvalidPassedData },
				{ "CustomProcessingError", ::Core::DataStream::Result::CustomProcessingError }
        };

        return map;
    }
}; // struct R<Core::DataStream::Result>

template<>
struct R<Core::DataStream::Mode>
{
    static constexpr std::string_view Name() { return "Mode"; }
    static constexpr std::size_t Size() { return 2; }
    static constexpr std::string_view ParentScope() { return "Core::DataStream"; }

    static std::string_view ToString(::Core::DataStream::Mode value)
    {
        const auto& data = R<Core::DataStream::Mode>::ToMapCN();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        static const std::string_view empty{};
        return empty;
    }

    static std::optional<::Core::DataStream::Mode> FromString(std::string_view value)
    {
        const auto& data = R<Core::DataStream::Mode>::ToMapNC();
        const auto it = data.find(value);
        if (it != data.end()) [[likely]]
        {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const std::array<::Core::DataStream::Mode, 2>& ToArrayC()
    {
        static constexpr std::array<::Core::DataStream::Mode, 2> constants = {
				::Core::DataStream::Mode::Input,
				::Core::DataStream::Mode::Output
        };

        return constants;
    }

    static constexpr const std::array<std::string_view, 2>& ToArrayN()
    {
        static constexpr std::array<std::string_view, 2> names = {
				std::string_view("Input"),
				std::string_view("Output")
        };

        return names;
    }

    static const std::unordered_map<::Core::DataStream::Mode, std::string_view>& ToMapCN()
    {
        static const std::unordered_map<::Core::DataStream::Mode, std::string_view> map = {
				{ ::Core::DataStream::Mode::Input, "Input" },
				{ ::Core::DataStream::Mode::Output, "Output" }
        };

        return map;
    }

    static const std::unordered_map<std::string_view, ::Core::DataStream::Mode>& ToMapNC()
    {
        static const std::unordered_map<std::string_view, ::Core::DataStream::Mode> map = {
				{ "Input", ::Core::DataStream::Mode::Input },
				{ "Output", ::Core::DataStream::Mode::Output }
        };

        return map;
    }
}; // struct R<Core::DataStream::Mode>

// clang-format on