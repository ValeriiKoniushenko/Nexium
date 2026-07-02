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

#include "ModalAssetsSearchPopUp.h"

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/List.h"
#include "Editor/GuiComponents/Separator.h"
#include "Editor/GuiComponents/Spacer.h"
#include "GameplaySystem/Framework/GameInstance.h"

namespace Core
{
    ECS_COMPONENT_IMPL(ModalAssetsSearchPopUpEWC);

    void ModalAssetsSearchPopUpEWC::open(StringAtom text)
    {
        initialize();
        enable();
        if (_hasOpenRequest)
        {
            warnLog(
                "Can't open second time ModalAssetsSearchPopUpEWC. It's already processing the "
                "request.");
            return;
        }
        _caption = std::move(text);
        _hasOpenRequest = true;
    }

    void ModalAssetsSearchPopUpEWC::Open(StringAtom text)
    {
        GetEditor().tryToOpenWindow<ModalAssetsSearchPopUpEWC>(".*", std::move(text));
    }

    void ModalAssetsSearchPopUpEWC::onInitialize()
    {
        BaseEWC::onInitialize();

        _layout.setHorizontalAlign(Gui::Align::Center);
        _layout.setFlex(Gui::Flex::FlexWidthAndHeight);

        {
            _nameField = _layout.addChildComponent<Gui::LabelRow<Gui::TextInput>>();
            _nameField->label->setText("Name");
            _nameField->label->setWidth(80.f);
            _nameField->input->setFlex(Gui::Flex::FlexWidth);
            _nameField->input->onInput->subscribe([this](const char* data)
                                                  { _wasManuallyEdited = true; });
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
                    if (auto asset
                        = GetAssetsManager().getWeakEcsAssetAt(index, Tag_WorldObject).tryLoad())
                    {
                        out = asset->getName();
                        return asset->getName().c_str();
                    }

                    Assert(false);
                    return nullptr;
                });
            _list->setSizeProvider(
                []() { return GetAssetsManager().getEcsAssetCountByTag(Tag_WorldObject); });
            _subscriptionPool << _list->onSelect->subscribeAndGetID(
                [this](const void*, StringAtom name)
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
    }

    void ModalAssetsSearchPopUpEWC::onDraw()
    {
        _layout.tick(GetWorld().timeDelta);

        ImGui::Dummy({});

        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
        {
            closeWindow();
        }
    }

    void ModalAssetsSearchPopUpEWC::preOpenedEndWindowDraw()
    {
        ImGui::EndPopup();
    }

    bool ModalAssetsSearchPopUpEWC::beginWindowDraw()
    {
        if (_hasOpenRequest)
        {
            ImGui::OpenPopup(_caption.c_str());
            ImGui::SetNextWindowSize(glm::vec2(500, 600), ImGuiCond_Appearing);
            _hasOpenRequest = false;
        }
        return ImGui::BeginPopupModal(_caption.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
    }

    void ModalAssetsSearchPopUpEWC::endWindowDraw()
    {
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
            = GetAssetsManager().getWeakEcsAssetAt(_list->getCurrentIndex(), Tag_WorldObject);

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
        BaseEWC::onClose();

        _wasManuallyEdited = false;
    }
} // namespace Core