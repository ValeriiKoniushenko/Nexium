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

#include "EditorWindow.h"
#include "ImGui/imgui.h"
#include "Misc/BaseLog.h"
#include "ModuleInfo.h"
#include "Utils/CopyableAndMoveableBehaviour.h"

namespace SW
{
    class GameEditor : public Utils::NotCopyableAndNotMoveable, public BaseLog
    {
    public:
        inline static std::filesystem::path defaultImGuiFontPath
            = "assets/fonts/JetBrainsMono-Regular.ttf";
        inline static float defaultImGuiFontSize = 16.f;
        inline static int defaultIoConfigFlagImGui = ImGuiConfigFlags_NavEnableKeyboard;

        GameEditor() = default;
        ~GameEditor() override;

        void initialize();

        void onTick(float delta);
        [[nodiscard]] bool isEnabled() const noexcept { return _isEnabled; }
        void setIsEnabled(bool v) noexcept { _isEnabled = v; }

        template<IsEditorWindowComponent T>
        [[nodiscard]] typename T::Ptr addNewWindow(const Core::StringAtom& name = ""_atom)
        {
            auto& a = _windows.emplace_back(new T);
            a->initialize();
            if (name)
            {
                a->setComponentName(name);
            }
            return boost::static_pointer_cast<T>(a);
        }

        template<IsEditorWindowComponentOrBase WindowT = BaseEditorWindowComponent>
        [[nodiscard]] WindowT* getWindow(const Core::StringAtom& regexName)
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

        template<IsEditorWindowComponentOrBase WindowT = BaseEditorWindowComponent>
        void showWindow(const Core::StringAtom& regexName)
        {
            if (auto* wnd = getWindow<WindowT>(regexName))
            {
                wnd->setEnabled(true);
            }
        }

        /**
         * @brief Totally destroy the object. Will called automatically at the destructor.
         */
        void destroy();

        [[nodiscard]] spdlog::logger* getLogger() const override { return SW::Editor::getLogger(); }

    protected:
        void setupImGuiStyles();
        [[nodiscard]] bool needToDraw();

    protected:
        std::vector<BaseEditorWindowComponent::Ptr> _windows;
        bool _isInitImGui = false;
        bool _isEnabled = true;
    };

} // namespace SW