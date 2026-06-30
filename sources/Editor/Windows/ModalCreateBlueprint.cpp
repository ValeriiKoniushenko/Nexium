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

#include "ModalCreateBlueprint.h"

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/List.h"
#include "Editor/GuiComponents/Separator.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Misc/Configs.h"

namespace Core
{
    ECS_COMPONENT_IMPL(ModalCreateBlueprintEWC);

    void ModalCreateBlueprintEWC::open(StringAtom text)
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
    }

    void ModalCreateBlueprintEWC::Open(StringAtom text)
    {
        GetEditor().tryToOpenWindow<ModalCreateBlueprintEWC>(".*", std::move(text));
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
                [this](const char* data) { tryToDetectReplacingOfExistingFile(); });
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
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();
            _pathField = h->addChildComponent<Gui::LabelRow<Gui::TextInput>>();
            _pathField->label->setText("Path");
            _pathField->label->setWidth(80.f);
            _pathField->input->setFlex(Gui::Flex::FlexWidth);
            _pathField->input->setInputtedData(Config::Path::assets.generic_string());
            _subscriptionPool << _pathField->input->onInput->subscribeAndGetID(
                [this](const char* data) { tryToDetectReplacingOfExistingFile(); });
        }

        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();
            _errorOutput = h->addChildComponent<Gui::Label>();
            _errorOutput->setTextColor(Color4_Red);
            _errorOutput->setText("");
        }

        {
            _list = _layout.addChildComponent<Gui::ListModelBased>();
            _list->setFlex(Gui::Flex::FlexWidthAndHeight);
            _list->setHeight(100.f);
            _list->setDataProvider(
                [](std::size_t index, StringAtom& out) -> const void*
                {
                    out = GetGlobalComponentFactory().getRegisteredTypesAsVector(true).at(index);
                    return nullptr;
                });
            _list->setSizeProvider(
                []()
                { return GetGlobalComponentFactory().getRegisteredTypesAsVector(true).size(); });
            _subscriptionPool << _list->onSelect->subscribeAndGetID(
                [this](const void* data, StringAtom type)
                { _typeField->input->setInputtedData(type.toStdString()); });
        }

        _layout.addChildComponent<Gui::Separator>();

        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();

            _okButton = h->addChildComponent<Gui::Button>("Create");
            _okButton->setFlex(Gui::Flex::FlexWidth);
            _okButton->onClick->subscribe([this]() { okButtonClicked(); });

            _cancelButton = h->addChildComponent<Gui::Button>("Cancel");
            _cancelButton->setFlex(Gui::Flex::FlexWidth);
            _cancelButton->onClick->subscribe([this]() { cancelButtonClicked(); });
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

    void ModalCreateBlueprintEWC::onClose()
    {
        BaseEWC::onClose();
        resetGUI();
    }

    void ModalCreateBlueprintEWC::resetGUI()
    {
        if (_nameField)
        {
            _nameField->input->setInputtedData("");
            _nameField->input->resetBorderColor();
        }

        if (_typeField)
        {
            _typeField->input->setInputtedData("");
            _typeField->input->resetBorderColor();
        }

        if (_errorOutput)
        {
            _errorOutput->setText("");
        }

        if (_pathField)
        {
            _pathField->input->resetBorderColor();
        }

        if (_list)
        {
            _list->resetListNavigation();
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
                                                           const std::string& name,
                                                           const std::string& path)
    {
        namespace fs = std::filesystem;

        auto* tmp = GetGlobalComponentFactory().create(StringAtom::Intern(type));
        if (!tmp) [[unlikely]]
        {
            criticalLog("ECS issue. Can't create component of type: {}"_f << type);
            return;
        }

        tmp->setComponentName(StringAtom(name));

        ECSAsset asset(""_atom);
        ECSAsset::PackObjectToAsset(asset, tmp);
        auto&& j = asset.toJson().dump(4);

        auto namePath = fs::path(_nameField->input->getInputtedData() + ".nx");
        auto pathPath = fs::path(_pathField->input->getInputtedData());
        auto finalPath = pathPath / namePath;

        std::ofstream file(finalPath);
        if (!file.is_open()) [[unlikely]]
        {
            criticalLog("Impossible to create or open a file: {}"_f << finalPath.generic_string());
            return;
        }

        file.write(j.c_str(), static_cast<std::streamsize>(j.size()));
    }

    void ModalCreateBlueprintEWC::tryToDetectReplacingOfExistingFile()
    {
        namespace fs = std::filesystem;

        auto name = fs::path(_nameField->input->getInputtedData() + ".nx");
        auto path = fs::path(_pathField->input->getInputtedData());
        auto finalPath = path / name;

        if (fs::exists(finalPath))
        {
            _errorOutput->setText("Such file already exists! After creation it will be replaced!");
        }
        else
        {
            _errorOutput->setText("");
        }
    }

    void ModalCreateBlueprintEWC::cancelButtonClicked()
    {
        closeWindow();
    }

    void ModalCreateBlueprintEWC::okButtonClicked()
    {
        std::string error;

        _errorOutput->setText("");
        _nameField->input->resetBorderColor();
        _typeField->input->resetBorderColor();
        _pathField->input->resetBorderColor();

        if (_nameField->input->getInputtedData().empty())
        {
            _nameField->input->setBorderColor(Color4_Red);
            error = "Name is empty";
        }

        if (_typeField->input->getInputtedData().empty())
        {
            _typeField->input->setBorderColor(Color4_Red);
            error = "Type is empty";
        }
        else if (!GetGlobalComponentFactory().containsSuchType(
                     _typeField->input->getInputtedData().data()))
        {
            _typeField->input->setBorderColor(Color4_Red);
            error = "Type is not recognized in ECS system";
        }

        if (!std::filesystem::exists(_pathField->input->getInputtedData()))
        {
            error = "Provided path doesn't exist or incorrect";
            _pathField->input->setBorderColor(Color4_Red);
        }

        if (!error.empty())
        {
            _errorOutput->setText(error.c_str());
            return;
        }

        performBlueprintCreation(_typeField->input->getInputtedData(),
                                 _nameField->input->getInputtedData(),
                                 _pathField->input->getInputtedData());

        closeWindow();
        GetAssetsManager().refreshFilesSystem();
    }

} // namespace Core