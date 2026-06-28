/*
* MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "AssetsManagerWindow.h"
#include "Core/Delegate.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "Editor/Windows/BaseWindow.h"
#include "JustReflectMe/Adapter.h"

namespace Core
{
    namespace Gui
    {
        class Label;
        class Button;
        class TextInput;
    }

    CLASS();
    class RenamePopUpWindow : public BaseEWC
    {
        R_FRIEND_DECL (RenamePopUpWindow);
        ECS_COMPONENT_DECL(RenamePopUpWindow, BaseEWC);

    public:
        void open(const StringAtom& text, const std::filesystem::path& path, std::function<void(
                      const std::filesystem::path& oldPath,
                      const std::filesystem::path& newPath)> onRenameCallback);

        static void Open(const StringAtom& text, const std::filesystem::path& path,
                         std::function<void(
                             const std::filesystem::path& oldPath,
                             const std::filesystem::path& newPath)> onRenameCallback);

    protected:
        void onDraw() override;
        void onInitialize() override;
        void preOpenedEndWindowDraw() override;
        [[nodiscard]] bool beginWindowDraw() override;

        void endWindowDraw() override;

    private:
        void updateSelectedPathAfterRename();
        std::string TrimWhitespace(std::string value);
        bool ContainsInvalidFilenameCharacter(std::string_view value);
        bool renamePath(const std::string& newName);

    private:
        DelegateSubscriberPoolGuard _subscriptionPool;

        Gui::VerticalLayout _layout;

        Gui::Label* _label{nullptr};
        Gui::Button* _applyButton{nullptr};
        Gui::Button* _cancelButton{nullptr};
        Gui::TextInput* _fileNameInput{nullptr};

    private:
        std::function<void(const std::filesystem::path& oldPath,
                           const std::filesystem::path& newPath)> _onRenameCallback;

        StringAtom _caption = "ModalRenameFileName";

        std::string _renameBuffer;
        std::string _renameError;
        std::filesystem::path _renameToPath{};

        bool _hasOpenRequest = false;
    };
}

#include "RenamePopUpWindow.generated.h" // added by the code generator. Better don't move it.
