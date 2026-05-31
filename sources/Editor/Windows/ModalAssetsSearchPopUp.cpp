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
#include "GameplaySystem/Framework/GameInstance.h"

namespace Core
{
    ECS_COMPONENT_IMPL(ModalAssetsSearchPopUpEWC);

    void ModalAssetsSearchPopUpEWC::open(StringAtom text,
                                         const std::function<void(NXAsset)>& callback)
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
        _callback = callback;
    }

    void ModalAssetsSearchPopUpEWC::Open(StringAtom text,
                                         const std::function<void(NXAsset)>& callback)
    {
        GetEditor().tryToOpenWindow<ModalAssetsSearchPopUpEWC>(".*", std::move(text), callback);
    }

    void ModalAssetsSearchPopUpEWC::onInitialize()
    {
        BaseEWC::onInitialize();

        _layout.setHorizontalAlign(Gui::Align::Center);
        _layout.setFlex(Gui::Flex::FlexWidthAndHeight);

        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();
            auto* input = h->addChildComponent<Gui::TextInput>();
            input->setPlaceholder("Regex filter..");
            input->setFlex(Gui::Flex::FlexWidth);
            _subscriptionPool << input->onInput->subscribeAndGetID(
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
                [](std::size_t index, StringAtom& out) -> void*
                {
                    if (auto asset = GetAssetsManager().getWeakAssetAt(index, AA_Spawn).tryLoad())
                    {
                        out = asset->getName();
                        return asset.get();
                    }

                    Assert(false);
                    return nullptr;
                });
            _list->setSizeProvider([]() { return GetAssetsManager().getAssetCount(AA_Spawn); });
        }

        _layout.addChildComponent<Gui::Separator>();

        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();

            _okButton = h->addChildComponent<Gui::Button>("Add");
            _okButton->setFlex(Gui::Flex::FlexWidth);
            _okButton->onClick->subscribe(
                [this]()
                {
                    if (!Verify(!!_callback))
                    {
                        return;
                    }

                    /*auto comp
                        = GlobalComponentFactory::Instance().create(_listView->getCurrentData());
                    if (!Verify(comp))
                    {
                        criticalLog("Can't fetch the asset: {}"_f << _listView->getCurrentData());
                        return;
                    }

                    comp->setComponentName(_nameInput->input->getInputtedData().c_str());

                    closeWindow();

                    _callback(comp);*/
                });

            _cancelButton = h->addChildComponent<Gui::Button>("Cancel");
            _cancelButton->setFlex(Gui::Flex::FlexWidth);
            _cancelButton->onClick->subscribe([this]() { closeWindow(); });
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
} // namespace Core