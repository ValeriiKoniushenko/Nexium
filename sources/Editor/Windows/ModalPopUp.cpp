// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "ModalPopUp.h"

#include "GameplaySystem/Framework/GameInstance.h"

namespace Core
{

    ECS_COMPONENT_IMPL(ModalPopUp);

    void ModalPopUp::open(StringAtom text, const std::function<void(bool)>& okOrCancelCallback)
    {
        _text = std::move(text);
        _okOrCancelCallback = okOrCancelCallback;
        _hasOpenRequest = true;
    }

    void ModalPopUp::Open(StringAtom text, const std::function<void(bool)>& okOrCancelCallback)
    {
        GetEditor().getWindow<ModalPopUp>()->open(std::move(text), okOrCancelCallback);
    }

    void ModalPopUp::onInitialize()
    {
        BaseEWC::onInitialize();

        _okButton = _layout.addChildComponent<Gui::Button>("OK");
        _cancelButton = _layout.addChildComponent<Gui::Button>("Cancel");

        _okButton->setWidth(120.f);
        _cancelButton->setWidth(120.f);

        _layout.setHorizontalAlign(Gui::Widget::Align::Center);

        _okButton->onClick.subscribe(
            [this](Gui::Button*)
            {
                if (Verify(!!_okOrCancelCallback))
                {
                    _okOrCancelCallback(true);
                }
                ImGui::CloseCurrentPopup();
            });
        _cancelButton->onClick.subscribe(
            [this](Gui::Button*)
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

        _layout.tick(GetWorld().timeDelta);

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
