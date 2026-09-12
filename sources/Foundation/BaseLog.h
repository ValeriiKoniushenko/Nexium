// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

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

namespace Foundation
{

    class LogQueue final : public Core::Singleton<LogQueue>
    {
        SINGLETONS_FRIEND(LogQueue)
    public:
        ~LogQueue() override = default;

        struct LogLine
        {
            Core::StringAtom author;
            Core::StringAtom message;
            spdlog::level::level_enum level = {};
            std::time_t time = 0;

            [[nodiscard]] Core::StringAtom toString() const;
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

        BaseLog(const BaseLog&) = default;
        BaseLog(BaseLog&&) noexcept = default;
        BaseLog& operator=(const BaseLog&) = default;
        BaseLog& operator=(BaseLog&&) noexcept = default;

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

        void traceLog(const char* str) const { pushLog(level::trace, str); }
        void traceLog(const std::string& str) const { traceLog(str.c_str()); }
        void traceLog(const Core::StringAtom& str) const { traceLog(str.c_str()); }
        void traceLog(const Core::StringFormatter<char>& str) const { traceLog(str.c_str()); }

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

        void pushLog(level l, const char* str) const;

        [[nodiscard]] virtual const char* getPrefix() const { return nullptr; }

        [[nodiscard]] virtual spdlog::logger* getLogger() const = 0;

    protected:
        BaseLog() = default;

    private:
        [[nodiscard]] Core::StringAtom getCompleteText(const char* str) const;
    };

    class GlobalLog : public Foundation::BaseLog
    {
    public:
        [[nodiscard]] spdlog::logger* getLogger() const final;
    };

} // namespace Foundation

extern Foundation::GlobalLog gGlobalLog;