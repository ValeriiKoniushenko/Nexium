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

#include "BaseLog.h"

namespace SW
{

    GlobalLog globalLog = {};

    nlohmann::json LogsQueue::Log::toJson() const
    {
        nlohmann::json json;
        json["author"] = author;
        json["authorPrefix"] = authorPrefix;
        json["message"] = message;
        json["time"] = time;
        auto levelStr = spdlog::level::to_string_view(level);
        json["level"] = levelStr.data();

        return json;
    }

    void LogsQueue::Log::fromJson(const nlohmann::json& json)
    {
        author = json["author"].get<Core::StringAtom>();
        authorPrefix = json["authorPrefix"].get<Core::StringAtom>();
        message = json["message"].get<Core::StringAtom>();
        time = json["time"].get<std::time_t>();
        level = spdlog::level::from_str(json["level"].get<std::string>());
    }

    void LogsQueue::push(Log&& log)
    {
        std::unique_lock lg(_mutex);
        _q.push(std::move(log));
    }

    std::vector<LogsQueue::Log> LogsQueue::flush()
    {
        std::vector<LogsQueue::Log> logs;
        std::unique_lock lg(_mutex);
        logs.reserve(_q.size());

        while (!_q.empty())
        {
            logs.emplace_back(_q.front());
            _q.pop();
        }

        return logs;
    }

    LogsQueue::Log LogsQueue::frontAndPop()
    {
        std::unique_lock lg(_mutex);
        auto l = _q.front();
        _q.pop();
        return l;
    }

    bool LogsQueue::isEmpty() const
    {
        std::shared_lock sl(_mutex);
        return _q.empty();
    }

    std::size_t LogsQueue::size() const
    {
        std::shared_lock sl(_mutex);
        return _q.size();
    }

    void BaseLog::pushLog(level l, const char* str) const
    {
        auto* logger = getLogger();
        logger->log(level::info, getCompleteText(str).c_str());

        if (spdlog::should_log(l))
        {
            LogsQueue::Log log;
            log.author = logger->name();
            log.authorPrefix = getPrefix();
            log.message = str;
            log.time = std::time(nullptr);
            log.level = l;

            GetLogsQueue().push(std::move(log));
        }
    }

    void BaseLog::criticalThrowingLog(const char* str) const
    {
        criticalLog(str);
        throw std::runtime_error(str);
    }

    Core::StringAtom BaseLog::getCompleteText(const char* str) const
    {
        if (auto prefix = getPrefix())
        {
            return ("{} | {}"_f << prefix << str).data();
        }
        return str;
    }

} // namespace SW