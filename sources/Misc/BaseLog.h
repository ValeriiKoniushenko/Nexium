/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "Core/String.h"
#include "spdlog/common.h"

#include <queue>

#define LOG_ONCE_P(logProvider, level, ...)                                                        \
    do                                                                                             \
    {                                                                                              \
        static bool _ = true;                                                                      \
        if (_) [[unlikely]]                                                                        \
        {                                                                                          \
            logProvider.pushLog(level, __VA_ARGS__);                                               \
            _ = false;                                                                             \
        }                                                                                          \
    } while (0)

#define LOG_INFO_ONCE_P(logProvider, ...)  LOG_ONCE_P(logProvider, spdlog::level::info, __VA_ARGS__)
#define LOG_WARN_ONCE_P(logProvider, ...)  LOG_ONCE_P(logProvider, spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR_ONCE_P(logProvider, ...) LOG_ONCE_P(logProvider, spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL_ONCE_P(logProvider, ...)                                                      \
    LOG_ONCE_P(logProvider, spdlog::level::critical, __VA_ARGS__)
#define LOG_DEBUG_ONCE_P(logProvider, ...)                                                         \
    LOG_ONCE_P(logProvider, spdlog::level::debug, __VA_ARGS__)
#define LOG_TRACE_ONCE_P(logProvider, ...)                                                         \
    LOG_ONCE_P(logProvider, spdlog::level::trace, __VA_ARGS__)
#define LOG_ASSERT_ERROR_ONCE_P(logProvider, ...)                                                  \
    LOG_ONCE_P(logProvider, spdlog::level::err, __VA_ARGS__);                                      \
    Assert(false, __VA_ARGS__)
#define LOG_ASSERT_CRITICAL_ONCE_P(logProvider, ...)                                               \
    LOG_ONCE_P(logProvider, spdlog::level::critical, __VA_ARGS__);                                 \
    Assert(false, __VA_ARGS__)

#define LOG_ONCE(level, ...)                                                                       \
    do                                                                                             \
    {                                                                                              \
        static bool _ = true;                                                                      \
        if (_) [[unlikely]]                                                                        \
        {                                                                                          \
            pushLog(level, __VA_ARGS__);                                                           \
            _ = false;                                                                             \
        }                                                                                          \
    } while (0)

#define LOG_INFO_ONCE(...)     LOG_ONCE(spdlog::level::info, __VA_ARGS__)
#define LOG_WARN_ONCE(...)     LOG_ONCE(spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR_ONCE(...)    LOG_ONCE(spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL_ONCE(...) LOG_ONCE(spdlog::level::critical, __VA_ARGS__)
#define LOG_DEBUG_ONCE(...)    LOG_ONCE(spdlog::level::debug, __VA_ARGS__)
#define LOG_TRACE_ONCE(...)    LOG_ONCE(spdlog::level::trace, __VA_ARGS__)

#define LOG_ASSERT_ERROR_ONCE(...)                                                                 \
    LOG_ONCE(spdlog::level::err, __VA_ARGS__);                                                     \
    Assert(false, __VA_ARGS__)
#define LOG_ASSERT_CRITICAL_ONCE(...)                                                              \
    LOG_ONCE(spdlog::level::critical, __VA_ARGS__);                                                \
    Assert(false, __VA_ARGS__)

namespace spdlog
{
    class logger;
}

namespace Core
{

    class LogQueue final : public Singleton<LogQueue>
    {
        SINGLETONS_FRIEND(LogQueue)
    public:
        struct LogLine
        {
            StringAtom author;
            StringAtom message;
            spdlog::level::level_enum level = {};
            std::time_t time = 0;

            [[nodiscard]] StringAtom toString() const;
        };

        [[nodiscard]] bool isEmpty() const { return _q.empty(); }
        void addLog(LogLine&& log) { _q.push(std::move(log)); }

        [[nodiscard]] LogLine frontAndPop()
        {
            auto out = _q.front();
            _q.pop();
            return out;
        }

    private:
        std::queue<LogLine> _q;
    };

    class BaseLog
    {
    public:
        using level = spdlog::level::level_enum;

        virtual ~BaseLog() = default;

        void infoLog(const char* str) const { pushLog(level::info, str); }

        void infoLog(const std::string& str) const { infoLog(str.c_str()); }

        void infoLog(const StringAtom& str) const { infoLog(str.c_str()); }

        void infoLog(const StringFormatter<char>& str) const { infoLog(str.c_str()); }

        void warnLog(const char* str) const { pushLog(level::warn, str); }

        void warnLog(const std::string& str) const { warnLog(str.c_str()); }

        void warnLog(const StringAtom& str) const { warnLog(str.c_str()); }

        void warnLog(const StringFormatter<char>& str) const { warnLog(str.c_str()); }

        void criticalLog(const char* str) const { pushLog(level::critical, str); }

        void criticalLog(const std::string& str) const { criticalLog(str.c_str()); }

        void criticalLog(const StringAtom& str) const { criticalLog(str.c_str()); }

        void criticalLog(const StringFormatter<char>& str) const { criticalLog(str.c_str()); }

        void errorLog(const char* str) const { pushLog(level::err, str); }

        void errorLog(const std::string& str) const { errorLog(str.c_str()); }

        void errorLog(const StringAtom& str) const { errorLog(str.c_str()); }

        void errorLog(const StringFormatter<char>& str) const { errorLog(str.c_str()); }

        void debugLog(const char* str) const { pushLog(level::debug, str); }

        void debugLog(const std::string& str) const { debugLog(str.c_str()); }

        void debugLog(const StringAtom& str) const { debugLog(str.c_str()); }

        void debugLog(const StringFormatter<char>& str) const { debugLog(str.c_str()); }

        void traceLog(const char* str) const { pushLog(level::trace, str); }

        void traceLog(const std::string& str) const { traceLog(str.c_str()); }

        void traceLog(const StringAtom& str) const { traceLog(str.c_str()); }

        void traceLog(const StringFormatter<char>& str) const { traceLog(str.c_str()); }

        void criticalThrowingLog(const char* str) const;

        void criticalThrowingLog(const std::string& str) const { criticalThrowingLog(str.c_str()); }

        void criticalThrowingLog(const StringAtom& s) const { criticalThrowingLog(s.c_str()); }

        void criticalThrowingLog(const StringFormatter<char>& s) const
        {
            criticalThrowingLog(s.c_str());
        }

        void pushLog(level l, const char* str) const;

        [[nodiscard]] virtual const char* getPrefix() const { return nullptr; }

        [[nodiscard]] virtual spdlog::logger* getLogger() const = 0;

    private:
        [[nodiscard]] StringAtom getCompleteText(const char* str) const;
    };

    class GlobalLog : public BaseLog
    {
    public:
        [[nodiscard]] spdlog::logger* getLogger() const final;
    };

    extern GlobalLog globalLog;

} // namespace Core
