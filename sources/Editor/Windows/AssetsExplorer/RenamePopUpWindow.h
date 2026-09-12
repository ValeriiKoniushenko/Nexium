// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Delegate.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "Editor/Windows/BaseWindow.h"
#include "JustReflectMe/Adapter.h"

#include <filesystem>
#include <functional>
#include <string>
#include <string_view>

namespace Core
{
    namespace Gui
    {
        class Label;
        class Button;
        class TextInput;
    } // namespace Gui

    CLASS();
    class RenamePopUpWindow : public BaseEWC
    {
        ECS_DECL(RenamePopUpWindow, Core::BaseEWC);

    public:
        using RenameCallbackT = std::function<void(const std::filesystem::path& oldPath,
                                                   const std::filesystem::path& newPath)>;

        struct ToastNotification
        {
            std::string text;
            float lifetime = 3.f;
            float elapsed = 0.f;
        };

    public:
        void open(const StringAtom& text, const std::filesystem::path& path,
                  RenameCallbackT onRenameCallback);

        static void Open(const StringAtom& text, const std::filesystem::path& path,
                         RenameCallbackT onRenameCallback);

    protected:
        void onDraw() override;
        void onInitialize() override;
        void preOpenedEndWindowDraw() override;
        [[nodiscard]] bool beginWindowDraw() override;

        void endWindowDraw() override;
        void onClose() override;

        void applyChangesAndCloseWindow();
        void cancelChangesAndCloseWindow();

    private:
        [[nodiscard]] bool ContainsInvalidFilenameCharacter(std::string_view value);
        [[nodiscard]] bool renamePath(const std::string& newName);

    private:
        DelegateSubscriberPoolGuard _subscriptionPool;

        Gui::VerticalLayout _layout;

        Gui::Label* _label{ nullptr };
        Gui::Button* _applyButton{ nullptr };
        Gui::Button* _cancelButton{ nullptr };
        Gui::TextInput* _fileNameInput{ nullptr };
        RenameCallbackT _onRenameCallback;

        StringAtom _caption = "ModalRenameFileName";

        std::string _renameBuffer;
        std::string _renameError;
        std::filesystem::path _renameToPath;
        bool _hasOpenRequest = false;
    };
} // namespace Core

#include "RenamePopUpWindow.generated.h" // added by the code generator. Better don't move it.
