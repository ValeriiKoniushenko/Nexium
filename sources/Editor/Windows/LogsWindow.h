// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BaseWindow.h"
#include "Editor/GuiComponents/HorizontalLayout.h"

namespace Core
{
    namespace Gui
    {
        class ToggleButton;
        class Button;
        class TextInput;
    } // namespace Gui

    CLASS();
    class LogsWindowEWC : public BaseFloatEWC
    {
        ECS_DECL_NO_SER(LogsWindowEWC, Core::BaseFloatEWC);

    public:
        struct LogLine
        {
            StringAtom message;
            spdlog::level::level_enum level = {};
        };

    public:
        void addLog(StringAtom&& log, spdlog::level::level_enum level);

        void clearLogs();

        [[nodiscard]] const char* getIcon() override;

        [[nodiscard]] nlohmann::json serialize() const override;
        void deserialize(RResourceStream<RJsonResourceStream>& stream) override;

    protected:
        void onPreInitialize() override;

        void onDraw() override;

        void onUpdate() override;

    private:
        void detectManualScroll();

        void fetchLogs();

        void logsDraw();

        [[nodiscard]] float getFitLogsCountOnScreen() const;

        [[nodiscard]] bool canBeFiltered(const StringAtom& msg);

    private:
        DelegateSubscriberPoolGuard _subscriptionPool;

        // ========= Toolbar ==========
        Gui::HorizontalLayout _toolbar;

        // FIELD(RFieldGen::NoDefaultValue);
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

        static const std::unordered_map<spdlog::level::level_enum, Color4> _levelColor;

        FIELD();
        std::size_t _logLimit = 500;

        float _betweenLogsSpace = 0.f;
        float _lastLogAreaHeight = 100.f;

        float _lastScrollY = 0.f;
        float _lastScrollMaxY = 0.f;
        float _lastScrollPercent = 0.f;

        bool _needScroll = false;
        std::deque<LogLine> _logs;
    };
} // namespace Core

#include "LogsWindow.generated.h" // added by the code generator. Better don't move it.
