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

    nlohmann::json LogsPool::Log::toJson() const
    {
        nlohmann::json json;
        json["author"] = author;
        json["authorPrefix"] = authorPrefix;
        json["message"] = message;
        json["time"] = time;
        json["level"] = spdlog::level::to_string_view(level);

        return json;
    }

    void LogsPool::Log::fromJson(const nlohmann::json& json)
    {
        author = json["author"].get<Core::StringAtom>();
        authorPrefix = json["authorPrefix"].get<Core::StringAtom>();
        message = json["message"].get<Core::StringAtom>();
        time = json["time"].get<std::time_t>();
        level = spdlog::level::from_str(json["level"].get<std::string>());
    }

    void BaseLog::pushLog(level l, const char* str) const
    {
        auto* logger = getLogger();
        logger->log(level::info, getCompleteText(str).c_str());

        if (spdlog::should_log(l))
        {
            LogsPool::Log log;
            log.author = logger->name();
            log.authorPrefix = getPrefix();
            log.message = str;
            log.time = std::time(nullptr);
            log.level = l;
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