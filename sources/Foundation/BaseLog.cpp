// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "BaseLog.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <shared_mutex>

Foundation::GlobalLog gGlobalLog = {};

namespace Foundation
{

    void BaseLog::pushLog(level l, const char* str) const
    {
        auto* logger = getLogger();
        auto log = getCompleteText(str);
        logger->log(l, log.c_str());

        LogQueue::LogLine logLine;
        logLine.level = l;
        logLine.author = getLogger()->name().c_str();
        logLine.message = std::move(log);
        logLine.time = std::time(nullptr);

        LogQueue::Instance().addLog(std::move(logLine));
        // Assert(l != level::critical, str);
    }

    spdlog::logger* GlobalLog::getLogger() const
    {
        const static std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("Global");
        return logger.get();
    }

    Core::StringAtom LogQueue::LogLine::toString() const
    {
        const std::time_t rawTime = std::time(nullptr);
        std::tm tm_struct{};
#if defined(_MSC_VER)
        localtime_s(&tm_struct, &rawTime);
#else
        tm_struct = *std::localtime(&rawTime);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tm_struct, "%H:%M:%S");

        return ("{} [{}] [{}] {}"_f << oss.str() << spdlog::level::to_short_c_str(level) << author
                                    << message)
            .data();
    }

    void BaseLog::criticalThrowingLog(const char* str) const
    {
        criticalLog(str);
        throw std::runtime_error(str);
    }

    Core::StringAtom BaseLog::getCompleteText(const char* str) const
    {
        if (const auto* const prefix = getPrefix())
        {
            return ("{} | {}"_f << prefix << str).data();
        }
        return str;
    }
} // namespace Foundation