/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "BaseLog.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <shared_mutex>


#include "BaseLog.generated.cpp.inl" // this line added by the code generator.

namespace Core
{
    GlobalLog globalLog = {};

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
        Assert(l != level::critical, str);
    }

    spdlog::logger* GlobalLog::getLogger() const
    {
        static std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("Global");
        return logger.get();
    }

    StringAtom LogQueue::LogLine::toString() const
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

    StringAtom BaseLog::getCompleteText(const char* str) const
    {
        if (const auto* const prefix = getPrefix())
        {
            return ("{} | {}"_f << prefix << str).data();
        }
        return str;
    }
} // namespace Core