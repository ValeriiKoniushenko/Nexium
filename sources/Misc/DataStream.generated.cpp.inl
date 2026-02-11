/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

namespace R
{

    namespace Core::DataStream::Result
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "Result"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::DataStream"; return name; }

        const std::string& ToString(::Core::DataStream::Result value)
        {
            const auto& data = R::Core::DataStream::Result::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static constexpr std::string empty{};
            return empty;
        }

        std::optional<::Core::DataStream::Result> FromString(const std::string& value)
        {
            const auto& data = R::Core::DataStream::Result::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::DataStream::Result, 5>& ToArrayC()
        {
            static const std::array<::Core::DataStream::Result, 5> constants = {
				::Core::DataStream::Result::Success,
				::Core::DataStream::Result::ReadFailed,
				::Core::DataStream::Result::WriteFailed,
				::Core::DataStream::Result::InvalidPassedData,
				::Core::DataStream::Result::CustomProcessingError
            };

            return constants;
        }

        const std::array<std::string, 5>& ToArrayN()
        {
            static const std::array<std::string, 5> names = {
				std::string("Success"),
				std::string("ReadFailed"),
				std::string("WriteFailed"),
				std::string("InvalidPassedData"),
				std::string("CustomProcessingError")
            };

            return names;
        }

        const std::unordered_map<::Core::DataStream::Result, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::DataStream::Result, std::string> map = {
				{ ::Core::DataStream::Result::Success, "Success" },
				{ ::Core::DataStream::Result::ReadFailed, "ReadFailed" },
				{ ::Core::DataStream::Result::WriteFailed, "WriteFailed" },
				{ ::Core::DataStream::Result::InvalidPassedData, "InvalidPassedData" },
				{ ::Core::DataStream::Result::CustomProcessingError, "CustomProcessingError" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::DataStream::Result>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::DataStream::Result> map = {
				{ "Success", ::Core::DataStream::Result::Success },
				{ "ReadFailed", ::Core::DataStream::Result::ReadFailed },
				{ "WriteFailed", ::Core::DataStream::Result::WriteFailed },
				{ "InvalidPassedData", ::Core::DataStream::Result::InvalidPassedData },
				{ "CustomProcessingError", ::Core::DataStream::Result::CustomProcessingError }
            };

            return map;
        }

    } // namespace Core::DataStream::Result

    namespace Core::DataStream::Mode
    {

        // =================== IMPLEMENTATIONS =====================
        const std::string& Name() { static const std::string name = "Mode"; return name; }
        const std::string& ParentScope() { static const std::string name = "Core::DataStream"; return name; }

        const std::string& ToString(::Core::DataStream::Mode value)
        {
            const auto& data = R::Core::DataStream::Mode::ToMapCN();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            static constexpr std::string empty{};
            return empty;
        }

        std::optional<::Core::DataStream::Mode> FromString(const std::string& value)
        {
            const auto& data = R::Core::DataStream::Mode::ToMapNC();
            const auto it = data.find(value);
            if (it != data.end()) [[likely]]
            {
                return it->second;
            }
            return std::nullopt;
        }

        const std::array<::Core::DataStream::Mode, 2>& ToArrayC()
        {
            static const std::array<::Core::DataStream::Mode, 2> constants = {
				::Core::DataStream::Mode::Input,
				::Core::DataStream::Mode::Output
            };

            return constants;
        }

        const std::array<std::string, 2>& ToArrayN()
        {
            static const std::array<std::string, 2> names = {
				std::string("Input"),
				std::string("Output")
            };

            return names;
        }

        const std::unordered_map<::Core::DataStream::Mode, std::string>& ToMapCN()
        {
            static const std::unordered_map<::Core::DataStream::Mode, std::string> map = {
				{ ::Core::DataStream::Mode::Input, "Input" },
				{ ::Core::DataStream::Mode::Output, "Output" }
            };

            return map;
        }

        const std::unordered_map<std::string, ::Core::DataStream::Mode>& ToMapNC()
        {
            static const std::unordered_map<std::string, ::Core::DataStream::Mode> map = {
				{ "Input", ::Core::DataStream::Mode::Input },
				{ "Output", ::Core::DataStream::Mode::Output }
            };

            return map;
        }

    } // namespace Core::DataStream::Mode


} // namespace
