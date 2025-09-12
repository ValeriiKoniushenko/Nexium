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

#include "Editor/Configs.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Spacer.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Misc/IconsFontAwesome.h"

namespace Core
{
    ECS_COMPONENT_IMPL(LogsWindowEWC)

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

    void LogsWindowEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        // Structure & ordering
        _searchInput = _toolbar.addChildComponent<Gui::TextInput>();
        _regexModeButton = _toolbar.addChildComponent<Gui::ToggleButton>(".*");
        _toolbar.addChildComponent<Gui::Spacer>();

        auto* levelHolder = _toolbar.addChildComponent<Gui::HorizontalLayout>("LogLevels");
        for (auto& [severity, ptr] : _levelFilter)
        {
            ptr = levelHolder->addChildComponent<Gui::ToggleButton>(
                spdlog::level::to_short_c_str(severity));
        }

        _toolbar.addChildComponent<Gui::Spacer>();
        _autoScrollButton = _toolbar.addChildComponent<Gui::ToggleButton>(ICON_FA_ARROW_DOWN);
        _clearButton = _toolbar.addChildComponent<Gui::Button>(ICON_FA_TRASH);

        // Styles
        levelHolder->setFlex(Gui::Widget::Flex::Fixed);

        _searchInput->setFlex(Gui::Widget::Flex::FlexWidth);
        _toolbar.setFlex(Gui::Widget::Flex::FlexWidth);

        tryReadFromCache();
    }

    void LogsWindowEWC::onDraw()
    {
        detectManualScroll();
        _toolbar.tick(GetWorld().timeDelta);

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
        const auto defaultSpace = ImGui::GetStyle().ItemSpacing.x;
        const auto finalWidth = ImGui::GetContentRegionAvail().x - defaultSpace;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + defaultSpace);

        if (ImGui::BeginChild("ScrollingRegion", glm::vec2(finalWidth, 0), 0,
                              ImGuiWindowFlags_HorizontalScrollbar))
        {
            bool justAdded = _lastCountOfLogs != _logs.size();
            if (!_isAutoScroll)
            {
                justAdded = false;
            }
            _lastCountOfLogs = _logs.size();

            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, glm::vec2(0, _betweenLogsSpace));

            auto w = getFitLogsCountOnScreen();

            for (std::size_t i = 0; i < _logs.size(); ++i)
            {
                auto& message = _logs[i].message;
                auto level = _logs[i].level;

                if (_levelFilter[level] && !_levelFilter[level]->isActive())
                {
                    continue;
                }

                if (_filterBuf[0] != '\0')
                {
                    if (_regexModeButton->isActive()
                        && message.regexFind(_filterBuf, 0, 0, 0, PCRE2_CASELESS))
                    {
                        continue;
                    }
                    else if (message.find(_filterBuf))
                    {
                        continue;
                    }
                }

                ImGui::PushStyleColor(ImGuiCol_Text, NormColor4::From(_levelColor.at(level)));

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                ImGui::PushID(static_cast<int>(i));

                ImGui::PushItemWidth(-FLT_MIN);
                ImGui::InputText("", message.data(), message.size() + 1,
                                 ImGuiInputTextFlags_ReadOnly);
                ImGui::PopItemWidth();

                ImGui::PopID();

                ImGui::PopStyleColor();
                ImGui::PopStyleVar();

                if (justAdded && i + 1 == _logs.size())
                {
                    ImGui::SetScrollHereY(1.0f);
                }
                ++i;
            }

            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(4);

            /*while (_logs.size() > _logLimit)
            {
                _logs.pop_front();
            }*/

            _lastLogAreaHeight = ImGui::GetWindowSize().y;
            if (!Math::IsZero(ImGui::GetScrollY()))
            {
                _lastScrollPercent = ImGui::GetScrollY() / ImGui::GetScrollMaxY();
            }
            else
            {
                _lastScrollPercent = 0.0f;
            }
        }
        ImGui::EndChild();
    }

    std::size_t LogsWindowEWC::getFitLogsCountOnScreen() const
    {
        const auto inputHeight = ImGui::GetTextLineHeight()
                                 + ImGui::GetStyle().FramePadding.y * 2.0f + _betweenLogsSpace;

        return std::ceil(_lastLogAreaHeight / inputHeight);
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
        for (auto [level, button] : _levelFilter)
        {
            nlohmann::json tmp;
            tmp["level"] = std::string(spdlog::level::to_string_view(level).data());
            tmp["value"] = button->isActive();
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
            for (const auto& filter : json["filterLevels"])
            {
                if (!filter.contains("level") || !filter.contains("value"))
                {
                    continue;
                }

                auto level = spdlog::level::from_str(filter["level"].get<std::string>());
                if (Verify(_levelFilter[level]))
                {
                    _levelFilter[level]->setActive(filter["value"].get<bool>());
                }
            }
        }
    }
} // namespace Core
