// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Foundation/BaseLog.h"
#include "InputDevices/InputManager.h"
#include "ObjectPicker.h"
#include "ToastNotifications.h"
#include "Windows/BaseWindow.h"

namespace Core
{
    /// Core editor class managing GUI windows and editor lifecycle.
    ///
    /// Provides initialization, ticking, and management of editor windows.
    /// Also handles font configuration and ImGui setup.
    class GameEditor : public Foundation::BaseLog
    {
    public:
        static const int defaultEditorImGuiFlags;

    public:
        GameEditor() = default;
        GameEditor& operator=(const GameEditor&) = delete;
        GameEditor& operator=(GameEditor&&) = delete;
        GameEditor(const GameEditor&) = delete;
        GameEditor(GameEditor&&) = delete;
        ~GameEditor() override;

        void initialize();

        void tick(float delta);

        [[nodiscard]] bool isEnabled() const noexcept { return _isEnabled; }
        void setIsEnabled(bool v) noexcept { _isEnabled = v; }

        /// Register a new editor window of type T.
        /// Initializes it, optionally sets its name, and enables it.
        /// @tparam T Type of the editor window component.
        /// @param name Optional name for the window.
        /// @param isEnabled Whether the window is enabled initially.
        /// @return Shared a pointer to the newly registered window.
        template<IsEditorWindowComponent T>
        T::Ptr registerNewWindow(StringAtom name, bool isEnabled = false)
        {
            if (!Verify(!!name))
            {
                return nullptr;
            }

            auto& a = _windows.emplace_back(new T);
            if (a->getIcon())
            {
                name = a->getIcon() + (" " + name);
            }
            a->setComponentName(std::move(name));

            if (isEnabled)
            {
                a->openWindow();
            }
            else
            {
                a->closeWindow();
            }

            return IntrusivePtr<T>(static_cast<T*>(a.get()));
        }

        /// Find a window by type and optional name regex.
        /// @tparam WindowT Type of window to search for (default is BaseEWC).
        /// @param regexName Regular expression to match the window title.
        /// @return Pointer to the first matching window or nullptr if none found.
        template<IsEditorWindowComponentOrBase WindowT>
        [[nodiscard]] WindowT* getWindow(const StringAtom& regexName = ".*")
        {
            for (auto&& windowIntrusive : _windows)
            {
                auto* wnd = windowIntrusive.get();
                if (!Verify(wnd))
                {
                    continue;
                }

                if (auto* casted = dynamic_cast<WindowT*>(wnd))
                {
                    if (casted->getWindowTitle().regexMatch(regexName))
                    {
                        return casted;
                    }
                }
            }

            return nullptr;
        }

        template<IsEditorWindowComponentOrBase WindowT, class... ArgsT>
        void tryToOpenWindow(const StringAtom& regexName = ".*", ArgsT&&... args)
        {
            auto* wnd = dynamic_cast<WindowT*>(getWindow<WindowT>(regexName));
            if (!wnd)
            {
                criticalLog(
                    "Can't get a window '{}'. Probably, this window type wasn't registered"_f
                    << WindowT::componentType);
                return;
            }
            wnd->open(std::forward<ArgsT>(args)...);
        }

        /// Show and enable a window matching the type and regex name.
        /// Optionally, pass string arguments to the window.
        /// @tparam WindowT Type of window to show (default is BaseEWC).
        /// @param regexName Regular expression to match the window title.
        /// @param args Optional arguments to pass to the window.
        template<IsEditorWindowComponentOrBase WindowT = BaseEWC>
        void showWindow(const StringAtom& regexName = ".*", const StringAtom& args = ""_atom)
        {
            if (auto* wnd = getWindow<WindowT>(regexName))
            {
                wnd->openWindow(args);
            }
        }

        /// Totally destroy the object. Will call automatically at the destructor.
        void destroy();

        [[nodiscard]] bool getIsRunSimulation() const noexcept { return _isRunSimulation; }
        void setIsRunSimulation(bool value) noexcept { _isRunSimulation = value; }
        void toggleSimulation() noexcept { _isRunSimulation = !_isRunSimulation; }

        [[nodiscard]] spdlog::logger* getLogger() const override;

        void writeToCache();
        void readFromCache();

    public:
        Editor::NotificationPopUpManager notifications;
        KeyboardInputManger keyboardInput;
        MouseInputManger mouseInput;
        RenderTargetToTexture gameViewport;

    protected:
        void setupImGuiStyles();
        [[nodiscard]] bool needToDraw();
        void setupShortcuts();

    private:
        void handleMouseDrag(glm::vec2 delta, MouseInputAction::SpecKeysState state);
        void handleMouseClick(glm::vec2 pos, MouseInputAction::SpecKeysState state);
        void responseOnPick(Transformable* object);
        void lazyOneShotInitialization();

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;
        std::vector<BaseEWC::Ptr> _windows;
        bool _isInitImGui = false;
        bool _isEnabled = true;
        bool _isRunSimulation = false;
    };
} // namespace Core
