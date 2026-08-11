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
    ECS_IMPL(ModalECSSearchPopUpEWC);

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

        onOpen();
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

        constexpr float defaultWidth = 80.f;

        {
            _nameInput = _layout.addChildComponent<Gui::LabelRow<Gui::TextInput>>();
            _nameInput->label->setText("Name");
            _nameInput->label->setWidth(defaultWidth);

            _nameInput->input->setPlaceholder("Put name here..");
            _nameInput->input->setFlex(Gui::Flex::FlexWidth);
            _subscriptionPool << _nameInput->input->onInput->subscribeAndGetID(
                [this](const char* data) { _nameInput->input->resetBorderColor(); });
            _subscriptionPool << _nameInput->input->onInput->subscribeAndGetID(
                [this](const char* data) { _wasManuallyEdited = true; });
        }

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
            static std::function<bool(Tag)> sortOuter
                = [](Tag tag) -> bool { return tag & (Tag_WorldObject | Tag_AnimationController); };

            _list = _layout.addChildComponent<Gui::ListModelBased>();
            _list->setFlex(Gui::Flex::FlexWidthAndHeight);
            _list->setDataProvider(
                [](std::size_t index, StringAtom& out) -> const void*
                {
                    auto data = GlobalComponentFactory::Instance().getRegisteredTypesAsVector(
                        true, sortOuter);
                    out = data.at(index);
                    return nullptr;
                });

            _list->setSizeProvider(
                []()
                {
                    return GlobalComponentFactory::Instance()
                        .getRegisteredTypesAsVector(false, sortOuter)
                        .size();
                });
            _subscriptionPool << _list->onSelect->subscribeAndGetID(
                [this](const void*, StringAtom name)
                {
                    if (!_wasManuallyEdited)
                    {
                        name.replaceAll("::", "-");
                        _nameInput->input->setInputtedData(name.toStdString());
                    }
                });

            _subscriptionPool << _list->onDoubleClickSelect->subscribeAndGetID(
                [this](const void*, StringAtom name) { okButtonClicked(); });
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
            _subscriptionPool << _cancelButton->onClick->subscribeAndGetID([this]()
                                                                           { closeWindow(); });
        }
    }

    void ModalECSSearchPopUpEWC::onDraw()
    {
        _layout.tick(GetWorld().getTimeDelta());

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

    void ModalECSSearchPopUpEWC::onClose()
    {
        BaseEWC::onClose();

        _wasManuallyEdited = false;
        if (_nameInput)
        {
            _nameInput->input->setInputtedData("");
        }

        if (_list)
        {
            _list->resetListNavigation();
        }
    }

    void ModalECSSearchPopUpEWC::onOpen()
    {
        BaseEWC::onOpen();

        if (_list)
        {
            _list->setKeyboardFocusAtStart();
        }
    }

    void ModalECSSearchPopUpEWC::okButtonClicked()
    {
        if (!Verify(_nameInput && _list && _callback))
        {
            return;
        }

        if (_nameInput->input->getInputtedData().empty())
        {
            _nameInput->input->setBorderColor(Color4_Red);
            return;
        }

        auto comp = GlobalComponentFactory::Instance().create(_list->tryGetCurrentDataAsString());
        if (!Verify(comp))
        {
            criticalLog("Can't create component: {}"_f << _list->tryGetCurrentDataAsString());
            return;
        }
        comp->setComponentName(_nameInput->input->getInputtedData().c_str());

        _callback(comp);

        closeWindow();
    }

    void ModalECSSearchPopUpEWC::cancelButtonClicked()
    {
        closeWindow();
    }
} // namespace Core