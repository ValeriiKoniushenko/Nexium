// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "ModalPopUp.h"

#include "Editor/EditorIntegration.h"
#include "Editor/GuiComponents/Button.h"
#include "Misc/IconsFontAwesome.h"

namespace Core
{
    ECS_IMPL(ModalPopUp);

    ModalPopUp::ModalPopUp(const Core::StringAtom& name)
        : BaseEWC(name),
          _caption(ICON_FA_TIMES " Warning!")
    {
    }

    void ModalPopUp::open(StringAtom text, const std::function<void(bool)>& okOrCancelCallback)
    {
        _text = std::move(text);
        _okOrCancelCallback = okOrCancelCallback;
        _hasOpenRequest = true;
    }

    void ModalPopUp::Open(StringAtom text, const std::function<void(bool)>& okOrCancelCallback)
    {
        GetEditor()->tryToOpenWindow<ModalPopUp>(".*", std::move(text), okOrCancelCallback);
    }

    void ModalPopUp::onInitialize()
    {
        BaseEWC::onInitialize();

        _okButton = _layout.addChildComponent<Gui::Button>("OK");
        _cancelButton = _layout.addChildComponent<Gui::Button>("Cancel");

        _okButton->setWidth(120.f);
        _cancelButton->setWidth(120.f);

        _layout.setHorizontalAlign(Gui::Align::Center);

        _subscriptionPool << _okButton->onClick->subscribeAndGetID(
            [this]()
            {
                if (Verify(!!_okOrCancelCallback))
                {
                    _okOrCancelCallback(true);
                }
                ImGui::CloseCurrentPopup();
            });
        _subscriptionPool << _cancelButton->onClick->subscribeAndGetID(
            [this]()
            {
                if (Verify(!!_okOrCancelCallback))
                {
                    _okOrCancelCallback(false);
                }
                ImGui::CloseCurrentPopup();
            });
    }

    void ModalPopUp::onDraw()
    {
        if (!Verify(!_text.isEmpty() && _okOrCancelCallback))
        {
            return;
        }

        ImGui::PushTextWrapPos(350.f);
        ImGui::TextWrapped("%s", _text.c_str());
        ImGui::PopTextWrapPos();

        ImGui::Dummy({});
        ImGui::Separator();
        ImGui::Dummy({});

        _layout.tick(GetWorld()->getTimeDelta());

        ImGui::Dummy({});
    }

    void ModalPopUp::preOpenedEndWindowDraw()
    {
        ImGui::EndPopup();
    }

    bool ModalPopUp::beginWindowDraw()
    {
        if (_hasOpenRequest)
        {
            ImGui::OpenPopup(_caption.c_str());
            _hasOpenRequest = false;
        }
        return ImGui::BeginPopupModal(_caption.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    }

    void ModalPopUp::endWindowDraw()
    {
    }
} // namespace Core
