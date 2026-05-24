/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "ModalCreateBlueprint.h"

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/List.h"
#include "Editor/GuiComponents/Separator.h"
#include "GameplaySystem/Framework/GameInstance.h"

namespace Core
{
    ECS_COMPONENT_IMPL(ModalCreateBlueprintEWC);

    void ModalCreateBlueprintEWC::open(StringAtom text, const std::function<void()>& callback)
    {
        initialize();
        enable();
        if (_hasOpenRequest)
        {
            warnLog(
                "Can't open second time ModalCreateBlueprintEWC. It's already processing the "
                "request.");
            return;
        }
        _caption = std::move(text);
        _hasOpenRequest = true;
        _callback = callback;
    }

    void ModalCreateBlueprintEWC::Open(StringAtom text, const std::function<void()>& callback)
    {
        GetEditor().tryToOpenWindow<ModalCreateBlueprintEWC>(".*", std::move(text), callback);
    }

    void ModalCreateBlueprintEWC::onInitialize()
    {
        BaseEWC::onInitialize();

        _layout.setHorizontalAlign(Gui::Align::Center);
        _layout.setFlex(Gui::Flex::FlexWidthAndHeight);

        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();
            _nameField = h->addChildComponent<Gui::LabelRow<Gui::TextInput>>();
            _nameField->label->setText("Name");
            _nameField->label->setWidth(80.f);
            _nameField->input->setFlex(Gui::Flex::FlexWidth);
            _subscriptionPool << _nameField->input->onInput->subscribeAndGetID(
                [this](const char* data)
                {
                    //
                });
        }

        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();
            _typeField = h->addChildComponent<Gui::LabelRow<Gui::TextInput>>();
            _typeField->label->setText("Base type");
            _typeField->label->setWidth(80.f);
            _typeField->input->setPlaceholder("Regex filter..");
            _typeField->input->setFlex(Gui::Flex::FlexWidth);
            _subscriptionPool << _typeField->input->onInput->subscribeAndGetID(
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
            _list->setHeight(100.f);
            _list->setDataProvider(
                [](std::size_t index, StringAtom& out) -> void*
                {
                    out = GetGlobalComponentFactory().getRegisteredTypesAsVector(true).at(index);
                    return nullptr;
                });
            _list->setSizeProvider(
                []()
                { return GetGlobalComponentFactory().getRegisteredTypesAsVector(true).size(); });
            _subscriptionPool << _list->onSelect->subscribeAndGetID(
                [this](void* data, StringAtom type)
                { _typeField->input->setInputtedData(type.toStdString()); });
        }

        _layout.addChildComponent<Gui::Separator>();

        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();

            _okButton = h->addChildComponent<Gui::Button>("Create");
            _okButton->setFlex(Gui::Flex::FlexWidth);
            _okButton->onClick->subscribe(
                [this]()
                {
                    if (!Verify(!!_callback))
                    {
                        return;
                    }

                    performBlueprintCreation(_typeField->input->getInputtedData(),
                                             _nameField->input->getInputtedData());
                });

            _cancelButton = h->addChildComponent<Gui::Button>("Cancel");
            _cancelButton->setFlex(Gui::Flex::FlexWidth);
            _cancelButton->onClick->subscribe([this]() { closeWindow(); });
        }
    }

    void ModalCreateBlueprintEWC::onDraw()
    {
        _layout.tick(GetWorld().timeDelta);

        ImGui::Dummy({});

        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
        {
            closeWindow();
        }
    }

    void ModalCreateBlueprintEWC::preOpenedEndWindowDraw()
    {
        ImGui::EndPopup();
    }

    bool ModalCreateBlueprintEWC::beginWindowDraw()
    {
        if (_hasOpenRequest)
        {
            ImGui::OpenPopup(_caption.c_str());
            ImGui::SetNextWindowSize(glm::vec2(500, 600), ImGuiCond_Appearing);
            _hasOpenRequest = false;
        }
        return ImGui::BeginPopupModal(_caption.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
    }

    void ModalCreateBlueprintEWC::endWindowDraw()
    {
    }

    void ModalCreateBlueprintEWC::performBlueprintCreation(const std::string& type,
                                                           const std::string& name)
    {
    }

} // namespace Core