// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "Core/StringHelper.h"
#include "JsonAdapter.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <Core/Enum.h>
#include <queue>
#include <shared_mutex>

namespace SW
{

    class BaseLog
    {
    public:
        using level = spdlog::level::level_enum;

        virtual ~BaseLog() = default;

        void infoLog(const char* str) const { pushLog(level::info, str); }

        void infoLog(const std::string& str) const { infoLog(str.c_str()); }

        void infoLog(const Core::StringAtom& str) const { infoLog(str.c_str()); }

        void infoLog(const Core::StringFormatter<char>& str) const { infoLog(str.c_str()); }

        void warnLog(const char* str) const { pushLog(level::warn, str); }

        void warnLog(const std::string& str) const { warnLog(str.c_str()); }

        void warnLog(const Core::StringAtom& str) const { warnLog(str.c_str()); }

        void warnLog(const Core::StringFormatter<char>& str) const { warnLog(str.c_str()); }

        void criticalLog(const char* str) const { pushLog(level::critical, str); }

        void criticalLog(const std::string& str) const { criticalLog(str.c_str()); }

        void criticalLog(const Core::StringAtom& str) const { criticalLog(str.c_str()); }

        void criticalLog(const Core::StringFormatter<char>& str) const { criticalLog(str.c_str()); }

        void errorLog(const char* str) const { pushLog(level::err, str); }

        void errorLog(const std::string& str) const { errorLog(str.c_str()); }

        void errorLog(const Core::StringAtom& str) const { errorLog(str.c_str()); }

        void errorLog(const Core::StringFormatter<char>& str) const { errorLog(str.c_str()); }

        void debugLog(const char* str) const { pushLog(level::debug, str); }

        void debugLog(const std::string& str) const { debugLog(str.c_str()); }

        void debugLog(const Core::StringAtom& str) const { debugLog(str.c_str()); }

        void debugLog(const Core::StringFormatter<char>& str) const { debugLog(str.c_str()); }

        void criticalThrowingLog(const char* str) const;

        void criticalThrowingLog(const std::string& str) const { criticalThrowingLog(str.c_str()); }

        void criticalThrowingLog(const Core::StringAtom& s) const
        {
            criticalThrowingLog(s.c_str());
        }

        void criticalThrowingLog(const Core::StringFormatter<char>& s) const
        {
            criticalThrowingLog(s.c_str());
        }

        [[nodiscard]] virtual const char* getPrefix() const { return nullptr; }
        [[nodiscard]] virtual spdlog::logger* getLogger() const = 0;

    private:
        [[nodiscard]] Core::StringAtom getCompleteText(const char* str) const;
        void pushLog(level l, const char* str) const;
    };

    class GlobalLog : public BaseLog
    {
    public:
        [[nodiscard]] spdlog::logger* getLogger() const final
        {
            static std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("Global");
            return logger.get();
        }
    };

    extern GlobalLog globalLog;

} // namespace SW