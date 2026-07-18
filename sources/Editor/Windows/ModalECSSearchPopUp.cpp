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

#include "ModalECSSearchPopUp.h"

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/List.h"
#include "Editor/GuiComponents/Separator.h"
#include "GameplaySystem/Framework/GameInstance.h"

namespace Core
{
    ECS_COMPONENT_IMPL(ModalECSSearchPopUpEWC);
    R_FRIEND_IMPL(ModalECSSearchPopUpEWC);

    void ModalECSSearchPopUpEWC::open(StringAtom text,
                                      const std::function<void(BaseComponent::Ptr)>& callback)
    {
        initialize();
        enable();
        if (_hasOpenRequest)
        {
            warnLog(
                "Can't open second time ModalECSSearchPopUpEWC. It's already processing the "
                "request.");
            return;
        }
        _caption = std::move(text);
        _hasOpenRequest = true;
        _callback = callback;
    }

    void ModalECSSearchPopUpEWC::Open(StringAtom text,
                                      const std::function<void(BaseComponent::Ptr)>& callback)
    {
        GetEditor().tryToOpenWindow<ModalECSSearchPopUpEWC>(".*", std::move(text), callback);
    }

    void ModalECSSearchPopUpEWC::onInitialize()
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
                    if (_listView && data)
                    {
                        _listView->setRegexFilter(StringAtom(data));
                    }
                });
        }

        {
            _listView = _layout.addChildComponent<Gui::ListView>();
            _listView->setFlex(Gui::Flex::FlexWidthAndHeight);
            auto data = GlobalComponentFactory::Instance().getRegisteredTypesAsVector();
            std::ranges::sort(data);
            _listView->setData(data);
        }

        {
            _nameInput = _layout.addChildComponent<Gui::LabelRow<Gui::TextInput>>();
            _nameInput->input->setPlaceholder("Put name here..");
            _nameInput->input->setFlex(Gui::Flex::FlexWidth);
            _subscriptionPool << _nameInput->input->onInput->subscribeAndGetID(
                [this](const char* data) { _nameInput->input->resetBorderColor(); });
        }

        _layout.addChildComponent<Gui::Separator>();

        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();

            _okButton = h->addChildComponent<Gui::Button>("Add");
            _okButton->setFlex(Gui::Flex::FlexWidth);
            _okButton->onClick->subscribe(
                [this]()
                {
                    if (!Verify(_nameInput && _callback))
                    {
                        return;
                    }

                    if (_nameInput->input->getInputtedData().empty())
                    {
                        _nameInput->input->setBorderColor(Color4_Red);
                        return;
                    }

                    auto comp
                        = GlobalComponentFactory::Instance().create(_listView->getCurrentData());
                    if (!Verify(comp))
                    {
                        criticalLog("Can't create component: {}"_f << _listView->getCurrentData());
                        return;
                    }

                    comp->setComponentName(_nameInput->input->getInputtedData().c_str());

                    closeWindow();

                    _callback(comp);
                });

            _cancelButton = h->addChildComponent<Gui::Button>("Cancel");
            _cancelButton->setFlex(Gui::Flex::FlexWidth);
            _cancelButton->onClick->subscribe([this]() { closeWindow(); });
        }
    }

    void ModalECSSearchPopUpEWC::onDraw()
    {
        _layout.tick(GetWorld().timeDelta);

        ImGui::Dummy({});

        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
        {
            closeWindow();
        }
    }

    void ModalECSSearchPopUpEWC::preOpenedEndWindowDraw()
    {
        ImGui::EndPopup();
    }

    bool ModalECSSearchPopUpEWC::beginWindowDraw()
    {
        if (_hasOpenRequest)
        {
            ImGui::OpenPopup(_caption.c_str());
            ImGui::SetNextWindowSize(glm::vec2(500, 600), ImGuiCond_Appearing);
            _hasOpenRequest = false;
        }
        return ImGui::BeginPopupModal(_caption.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
    }

    void ModalECSSearchPopUpEWC::endWindowDraw()
    {
    }
} // namespace Core