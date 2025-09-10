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

#include "BaseWindow.h"
#include "Misc/JsonCacheable.h"

#include <list>

namespace Core
{
    class LogsWindowEWC : public BaseFloatEWC, public JsonCacheable
    {
        ECS_COMPONENT_DECL(LogsWindowEWC, BaseFloatEWC);

    public:
        struct LogLine
        {
            StringAtom message;
            spdlog::level::level_enum level = {};
        };

    public:
        ~LogsWindowEWC() override;
        void addLog(StringAtom&& log, spdlog::level::level_enum level);
        void clearLogs();

    protected:
        std::filesystem::path getCacheDir() const override;
        StringAtom getCacheHash() const override;
        nlohmann::json toCacheData() const override;
        void fromCacheData(const nlohmann::json& json) override;

    protected:
        void onInitialize() override;
        void onDraw() override;
        void onUpdate() override;

    private:
        void detectManualScroll();
        void fetchLogs();
        void toolbarDraw();
        void logsDraw();

    private:
        inline static const std::vector<spdlog::level::level_enum> _levels = {
            spdlog::level::level_enum::critical, spdlog::level::level_enum::err,
            spdlog::level::level_enum::warn,     spdlog::level::level_enum::info,
            spdlog::level::level_enum::debug,    spdlog::level::level_enum::trace,
        };
        std::unordered_map<spdlog::level::level_enum, bool> _levelFilter = {
            { spdlog::level::level_enum::critical, true },
            { spdlog::level::level_enum::err, true },
            { spdlog::level::level_enum::warn, true },
            { spdlog::level::level_enum::info, true },
            { spdlog::level::level_enum::debug, true },
            { spdlog::level::level_enum::trace, true },
        };

        std::size_t _logLimit = 500;
        float _defaultGap = 4.f;
        float _clearButtonWidth = 0.f;
        float _autoScrollButtonWidth = 0.f;
        bool _isAutoScroll = true;
        float _streamingToolbarHeight = 40.f;
        float _toolbarToolsWidth = 150.f;
        StringAtom _filterBuf;
        std::size_t _lastCountOfLogs = 0;
        std::list<LogLine> _logs;
    };
} // namespace Core
