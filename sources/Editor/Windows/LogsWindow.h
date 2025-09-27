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

#pragma once

#include "BaseWindow.h"
#include "Editor/Configs.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
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

        [[nodiscard]] const char* getIcon() override { return ICON_FA_ALIGN_LEFT; }

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

        void logsDraw();

        [[nodiscard]] float getFitLogsCountOnScreen() const;

        [[nodiscard]] bool canBeFiltered(const StringAtom& msg);

    private:
        // ========= Toolbar ==========
        Gui::HorizontalLayout _toolbar;
        Gui::TextInput* _searchInput = nullptr;
        Gui::ToggleButton* _regexModeButton = nullptr;
        Gui::ToggleButton* _autoScrollButton = nullptr;
        Gui::Button* _clearButton = nullptr;
        // Log levels
        std::unordered_map<spdlog::level::level_enum, Gui::ToggleButton*> _levelFilter = {
            { spdlog::level::level_enum::critical, nullptr },
            { spdlog::level::level_enum::err, nullptr },
            { spdlog::level::level_enum::warn, nullptr },
            { spdlog::level::level_enum::info, nullptr },
            { spdlog::level::level_enum::debug, nullptr },
            { spdlog::level::level_enum::trace, nullptr },
        };

        inline static const std::unordered_map<spdlog::level::level_enum, Color4> _levelColor
            = { { spdlog::level::level_enum::critical, Config::ColorRed },
                { spdlog::level::level_enum::err, Config::ColorYellow },
                { spdlog::level::level_enum::warn, Config::ColorHalfYellow },
                { spdlog::level::level_enum::info, Config::ColorWhite },
                { spdlog::level::level_enum::debug, Config::ColorSoftWhite },
                { spdlog::level::level_enum::trace, Config::ColorGrey } };

        std::size_t _logLimit = 500;
        float _betweenLogsSpace = 0.f;
        float _lastLogAreaHeight = 100.f;

        float _lastScrollY = 0.f;
        float _lastScrollMaxY = 0.f;
        float _lastScrollPercent = 0.f;

        bool _needScroll = false;
        std::vector<LogLine> _logs;
    };
} // namespace Core