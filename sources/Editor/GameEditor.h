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

#include "InputDevices/InputManager.h"
#include "Misc/BaseLog.h"
#include "ModuleInfo.h"
#include "Utils/CopyableAndMoveableBehaviour.h"
#include "Windows/BaseWindow.h"

namespace Core
{

    /**
     * @brief Core editor class managing GUI windows and editor lifecycle.
     *
     * Provides initialization, ticking, and management of editor windows.
     * Also handles font configuration and ImGui setup.
     */
    class GameEditor : public Utils::NotCopyableAndNotMoveable, public BaseLog
    {
    public:
        inline static std::filesystem::path defaultImGuiFontPath
            = "assets/fonts/JetBrainsMono-Regular.ttf";
        inline static std::filesystem::path emojiImGuiFontPath
            = "assets/fonts/fontawesome-webfont.ttf";

        inline static float defaultImGuiFontSize = 16.f;
        inline static float emojiImGuiFontScale = 0.8f;

        inline static int defaultIoConfigFlagImGui
            = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;

        GameEditor() = default;
        ~GameEditor() override;

        void initialize();

        void onTick(float delta);
        [[nodiscard]] bool isEnabled() const noexcept { return _isEnabled; }
        void setIsEnabled(bool v) noexcept { _isEnabled = v; }

        /**
         * @brief Register a new editor window of type T.
         * Initializes it, optionally sets its name, and enables it.
         * @tparam T Type of the editor window component.
         * @param name Optional name for the window.
         * @param isEnabled Whether the window is enabled initially.
         * @return Shared pointer to the newly registered window.
         */
        template<IsEditorWindowComponent T>
        [[nodiscard]] typename T::Ptr registerNewWindow(const StringAtom& name = ""_atom,
                                                        bool isEnabled = true)
        {
            auto& a = _windows.emplace_back(new T);
            a->initialize();
            if (name)
            {
                a->setComponentName(name);
            }
            a->setEnabled(isEnabled);
            return boost::static_pointer_cast<T>(a);
        }

        /**
         * @brief Find a window by type and optional name regex.
         * @tparam WindowT Type of window to search for (default is BaseEWC).
         * @param regexName Regular expression to match window title.
         * @return Pointer to the first matching window or nullptr if none found.
         */
        template<IsEditorWindowComponentOrBase WindowT = BaseEWC>
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

        /**
         * @brief Show and enable a window matching the type and regex name.
         * Optionally pass string arguments to the window.
         * @tparam WindowT Type of window to show (default is BaseEWC).
         * @param regexName Regular expression to match window title.
         * @param args Optional arguments to pass to the window.
         */
        template<IsEditorWindowComponentOrBase WindowT = BaseEWC>
        void showWindow(const StringAtom& regexName = ".*", const StringAtom& args = ""_atom)
        {
            if (auto* wnd = getWindow<WindowT>(regexName))
            {
                wnd->setEnabled(true);
                wnd->putArguments(args);
            }
        }

        /**
         * @brief Totally destroy the object. Will called automatically at the destructor.
         */
        void destroy();

        [[nodiscard]] bool getIsRunSimulation() const noexcept { return _isRunSimulation; }
        void setIsRunSimulation(bool value) noexcept { _isRunSimulation = value; }
        void toggleSimulation() noexcept { _isRunSimulation = !_isRunSimulation; }

        [[nodiscard]] spdlog::logger* getLogger() const override { return Editor::getLogger(); }

    protected:
        void setupImGuiStyles();
        [[nodiscard]] bool needToDraw();

    protected:
        KeyboardInputManger _keyboardInput;
        MouseInputManger _mouseInput;

        std::vector<BaseEWC::Ptr> _windows;
        bool _isInitImGui = false;
        bool _isEnabled = true;
        bool _isRunSimulation = false;
    };

} // namespace Core