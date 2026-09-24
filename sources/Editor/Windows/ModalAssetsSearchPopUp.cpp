// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "ModalAssetsSearchPopUp.h"

#include "Editor/EditorIntegration.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/List.h"
#include "Editor/GuiComponents/Separator.h"
#include "Editor/GuiComponents/Spacer.h"
#include "Foundation/BaseLog.h"
#include "NxWorld/Framework/GameInstance.h"

using namespace NX;

namespace NX
{
    ECS_IMPL(ModalAssetsSearchPopUpEWC);

    void ModalAssetsSearchPopUpEWC::Open(StringAtom text)
    {
        GetEditor()->tryToOpenWindow<ModalAssetsSearchPopUpEWC>("", std::move(text));
    }

    void ModalAssetsSearchPopUpEWC::onInitialize()
    {
        BaseModalPopUp::onInitialize();
        setComponentName("Assets searcher"_atom);

        _layout.setHorizontalAlign(Gui::Align::Center);
        _layout.setFlex(Gui::Flex::FlexWidthAndHeight);

        {
            _nameField = _layout.addChildComponent<Gui::LabelRow<Gui::TextInput>>();
            _nameField->label->setText("Name");
            _nameField->label->setWidth(80.f);
            _nameField->input->setFlex(Gui::Flex::FlexWidth);
            _subscriptionPool << _nameField->input->onInput->subscribeAndGetID(
                [this](const char* data) { _wasManuallyEdited = true; });
        }

        _layout.addChildComponent<Gui::Spacer>();

        {
            auto* h = _layout.addChildComponent<Gui::TextInput>();
            h->setPlaceholder("Regex filter..");
            h->setFlex(Gui::Flex::FlexWidth);
            _subscriptionPool << h->onInput->subscribeAndGetID(
                [this](const char* data)
                {
                    if (_list && data)
                    {
                        _list->setRegexFilter(StringAtom(data));
                    }
                });
        }

        {
            _list = _layout.addChildComponent<Gui::ListModelBased>();
            _list->setFlex(Gui::Flex::FlexWidthAndHeight);
            _list->setDataProvider(
                [](std::size_t index, StringAtom& out) -> const void*
                {
                    if (auto asset = GetAssetsManager()->getECSAssetMeta(index, Tag_WorldObject))
                    {
                        out = asset->name;
                        return nullptr;
                    }

                    LOG_ERROR_ONCE_P(
                        gGlobalLog,
                        "ModalAssetsSearchPopUpEWC: Can't get asset meta at some index");
                    return nullptr;
                });
            _list->setSizeProvider(
                []() { return GetAssetsManager()->getEcsAssetCountByTag(Tag_WorldObject); });
            _subscriptionPool << _list->onSelect->subscribeAndGetID(
                [this](const void*, const StringAtom& name)
                {
                    if (!_wasManuallyEdited)
                    {
                        _nameField->input->setInputtedData(name.toStdString());
                    }
                });
        }

        _layout.addChildComponent<Gui::Separator>();

        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();

            _okButton = h->addChildComponent<Gui::Button>("Add");
            _okButton->setFlex(Gui::Flex::FlexWidth);
            _subscriptionPool << _okButton->onClick->subscribeAndGetID([this]()
                                                                       { okButtonClicked(); });

            _cancelButton = h->addChildComponent<Gui::Button>("Cancel");
            _cancelButton->setFlex(Gui::Flex::FlexWidth);
            _subscriptionPool << _cancelButton->onClick->subscribeAndGetID(
                [this]() { cancelButtonClicked(); });
        }

        if (_list)
        {
            _list->setKeyboardFocusAtStart();
        }
    }

    void ModalAssetsSearchPopUpEWC::onDraw()
    {
        BaseModalPopUp::onDraw();

        _layout.tick(GetWorld()->getTimeDelta());

        ImGui::Dummy({});

        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
        {
            cancelButtonClicked();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Enter, false))
        {
            okButtonClicked();
        }
    }

    void ModalAssetsSearchPopUpEWC::okButtonClicked()
    {
        StringAtom name = _nameField->input->getInputtedData().c_str();
        name.trim(' ');

        _nameField->input->resetBorderColor();
        if (name.isEmpty())
        {
            _nameField->input->setBorderColor(Color4_Red);
            return;
        }

        auto weakAsset
            = GetAssetsManager()->getWeakEcsAssetAt(_list->getCurrentIndex(), Tag_WorldObject);

        if (!weakAsset)
        {
            criticalLog("Impossible to create a scene object. The asset '{}' is inaccessible."_f
                        << _list->tryGetCurrentDataAsString());
            closeWindow();
            return;
        }

        auto loadedAsset = weakAsset.tryLoad();
        if (!loadedAsset)
        {
            criticalLog(
                "Impossible to create a scene object. The asset '{}' can't load it's own data."_f
                << _list->tryGetCurrentDataAsString());
            closeWindow();
            return;
        }

        gGameInstance->gameScene.addBlueprintObjectToScene(loadedAsset, name);

        closeWindow();
    }

    void ModalAssetsSearchPopUpEWC::cancelButtonClicked()
    {
        closeWindow();
    }

    void ModalAssetsSearchPopUpEWC::onClose()
    {
        BaseModalPopUp::onClose();

        _wasManuallyEdited = false;
        if (_nameField)
        {
            _nameField->input->setInputtedData("");
        }

        if (_list)
        {
            _list->resetListNavigation();
        }
    }
} // namespace NX
