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

#include "LogsWindow.h"

#include "Misc/IconsFontAwesome.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(LogsWindowEWC)

    //
    //     _
    //    | |
    //    | |     ___   __ _ ___
    //    | |    / _ \ / _` / __|
    //    | |___| (_) | (_| \__ \
    //    \_____/\___/ \__, |___/
    //                  __/ |
    //                 |___/
    // ========================================================================
    void LogsWindowEWC::addLog(StringAtom&& log, spdlog::level::level_enum level)
    {
        LogLine l;
        l.message = std::move(log);
        l.level = level;
        _logs.push_back(std::move(l));
    }

    void LogsWindowEWC::clearLogs()
    {
        _logs.clear();
    }

    void LogsWindowEWC::onInit()
    {
        BaseFloatEWC::onInit();

        tryReadFromCache();

        auto& style = ImGui::GetStyle();

        _clearButtonWidth = ImGui::CalcTextSize(ICON_FA_TRASH).x;
        _autoScrollButtonWidth = ImGui::CalcTextSize(ICON_FA_ARROW_DOWN).x;

        _defaultGap = style.ItemSpacing.x;
        _toolbarToolsWidth = 0;

        _filterBuf.resize(1024);

        // Calculating of tools from Toolbar.
        // Repeating or 'render' code from LogsWindowEWC::toolbarDraw
        _toolbarToolsWidth += _defaultGap * 3.f;
        for (auto level : _levels)
        {
            auto textSize = ImGui::CalcTextSize(spdlog::level::to_short_c_str(level)).x;
            textSize += style.FramePadding.x * 2.f;
            textSize += _defaultGap;
            _toolbarToolsWidth += textSize;
        }
        _toolbarToolsWidth += _defaultGap * 3.f;

        _toolbarToolsWidth += ImGui::CalcTextSize(ICON_FA_TRASH).x + style.FramePadding.x * 2.f;
        _toolbarToolsWidth += _defaultGap;

        _toolbarToolsWidth
            += ImGui::CalcTextSize(ICON_FA_ARROW_DOWN).x + style.FramePadding.x * 2.f;
        _toolbarToolsWidth += _defaultGap;
    }

    void LogsWindowEWC::onDraw()
    {
        detectManualScroll();
        toolbarDraw();
        logsDraw();
    }

    void LogsWindowEWC::onUpdate()
    {
        BaseFloatEWC::onUpdate();

        fetchLogs();
    }

    void LogsWindowEWC::fetchLogs()
    {
        auto& q = LogQueue::instance();
        while (!q.isEmpty())
        {
            auto qLog = q.frontAndPop();

            addLog(qLog.toString(), qLog.level);
        }
    }

    void LogsWindowEWC::logsDraw()
    {
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), 0,
                              ImGuiWindowFlags_HorizontalScrollbar))
        {
            bool justAdded = _lastCountOfLogs != _logs.size();
            if (!_isAutoScroll)
            {
                justAdded = false;
            }
            _lastCountOfLogs = _logs.size();

            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::Selectable("Clear"))
                {
                    clearLogs();
                }
                ImGui::EndPopup();
            }

            std::size_t i = 0;
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten _spacing
            for (auto& [message, level] : _logs)
            {
                std::optional<ImVec4> color;

                if (!_levelFilter[level])
                {
                    continue;
                }

                if (_filterBuf[0] != '\0'
                    && !message.regexFind(_filterBuf, 0, 0, 0, PCRE2_CASELESS))
                {
                    continue;
                }

                if (level == spdlog::level::critical)
                {
                    color = ColorRed;
                }
                if (level == spdlog::level::err)
                {
                    color = ColorYellow;
                }
                if (level == spdlog::level::warn)
                {
                    color = ColorHalfYellow;
                }
                if (level == spdlog::level::debug)
                {
                    color = ColorGrey;
                }

                if (color)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, *color);
                }

                ImGui::PushID(static_cast<int>(i));
                ImGui::PushItemWidth(-FLT_MIN); // Makes the next widget take full width
                ImGui::InputText("", message.data(), message.size() + 1,
                                 ImGuiInputTextFlags_ReadOnly);
                ImGui::PopItemWidth();
                ImGui::PopID();

                if (color)
                {
                    ImGui::PopStyleColor();
                }

                if (justAdded && i + 1 == _logs.size())
                {
                    ImGui::SetScrollHereY(1.0f);
                }
                ++i;
            }

            while (_logs.size() > _logLimit)
            {
                _logs.pop_front();
            }

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
    }

    void LogsWindowEWC::toolbarDraw()
    {
        float startY = ImGui::GetCursorScreenPos().y;
        ImGui::BeginChild("Toolbar", ImVec2(0, _streamingToolbarHeight));
        {
            ImGui::Dummy(ImVec2(0, 0));

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

            // =============== Input ====================
            ImGui::Dummy(ImVec2(_defaultGap, 0));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(_innerSize.width - _toolbarToolsWidth);
            ImGui::InputTextWithHint("##LogFilter",
                                     "Your filter message. Feel free to use regex(perl).",
                                     _filterBuf.data(), _filterBuf.size() + 1);
            ImGui::SameLine(0, _defaultGap * 3.f);

            // =============== Levels ====================
            for (auto level : _levels)
            {
                bool& status = _levelFilter[level];
                if (ToggleButton(spdlog::level::to_short_c_str(level), status))
                {
                    status = !status;
                }
                ImGui::SameLine(0, _defaultGap);
            }
            ImGui::SameLine(0, _defaultGap * 3.f);

            // =============== Clean logs ====================
            if (ImGui::Button(ICON_FA_TRASH))
            {
                clearLogs();
            }
            ImGui::SameLine(0, _defaultGap);

            // =============== AutoScroll ====================
            if (ToggleButton(ICON_FA_ARROW_DOWN, _isAutoScroll))
            {
                _isAutoScroll = !_isAutoScroll;
            }
            ImGui::SameLine(0, 0);
            ImGui::Dummy(ImVec2(_defaultGap, 0));

            ImGui::PopStyleVar();

            ImGui::Dummy(ImVec2(0, 0));
        }
        _streamingToolbarHeight = ImGui::GetCursorScreenPos().y - startY;
        ImGui::EndChild();
    }
    void LogsWindowEWC::detectManualScroll()
    {
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)
            && ImGui::GetIO().MouseWheel != 0.0f)
        {
            _isAutoScroll = false;
        }
    }

    LogsWindowEWC::~LogsWindowEWC()
    {
        writeToCache();
    }

    std::filesystem::path LogsWindowEWC::getCacheDir() const
    {
        return JsonCacheable::getCacheDir() / "editor";
    }

    StringAtom LogsWindowEWC::getCacheHash() const
    {
        return "LogsWindowEWC"_atom;
    }

    nlohmann::json LogsWindowEWC::toCacheData() const
    {
        nlohmann::json json;
        json["logLimit"] = _logLimit;
        json["filter"] = _filterBuf;
        json["filterLevels"] = nlohmann::json::array();
        for (auto [level, value] : _levelFilter)
        {
            nlohmann::json tmp;
            tmp["level"] = std::string(spdlog::level::to_string_view(level).data());
            tmp["value"] = value;
            json["filterLevels"].push_back(std::move(tmp));
        }

        return json;
    }

    void LogsWindowEWC::fromCacheData(const nlohmann::json& json)
    {
        if (json.contains("logLimit"))
        {
            _logLimit = json["logLimit"].get<decltype(_logLimit)>();
        }
        if (json.contains("filter"))
        {
            _filterBuf = json["filter"].get<decltype(_filterBuf)>();
        }
        if (json.contains("filterLevels"))
        {
            for (auto& filter : json["filterLevels"])
            {
                if (!filter.contains("level") || !filter.contains("value"))
                {
                    continue;
                }

                auto level = spdlog::level::from_str(filter["level"].get<std::string>());
                _levelFilter[level] = filter["value"].get<bool>();
            }
        }
    }
} // namespace Core
