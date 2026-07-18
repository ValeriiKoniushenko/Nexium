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

#pragma once

#include "../NxECSBasedEditor.h"
#include "Editor/GuiComponents/VerticalLayout.h"

namespace Core
{
    namespace Gui
    {
        template<class T>
        class LabelRow;

        class CheckBox;

        template<std::size_t Size, Utils::IsArithmetic Type>
        class VecNumInput;

        using Float3Input = VecNumInput<3, float>;
    } // namespace Gui

    CLASS();
    class ECSEditorActorAdapter : public ECSEditorMimeAdapter
    {
        R_FRIEND_DECL(ECSEditorActorAdapter, Core::ECSEditorMimeAdapter);
        ECS_COMPONENT_DECL(ECSEditorActorAdapter, ECSEditorMimeAdapter);

    public:
        [[nodiscard]] bool canWorkWith(BaseComponent* component) const override;
        [[nodiscard]] StringAtom getProcessedAssetType() const override;

    protected:
        void onApplyAssetData(const nlohmann::json& json) override;
        void onInitialize() override;
        void onDraw(float dt) override;

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;

        Gui::VerticalLayout _actorLayout;

        Gui::LabelRow<Gui::CheckBox>* _actorPostDraw = nullptr;
        Gui::LabelRow<Gui::Float3Input>* _actorPosition = nullptr;
        Gui::LabelRow<Gui::Float3Input>* _actorRotation = nullptr;
        Gui::LabelRow<Gui::Float3Input>* _actorScale = nullptr;
        Gui::LabelRow<Gui::Float3Input>* _actorOrigin = nullptr;
    };

} // namespace Core

#include "EditorActorAdapter.generated.h" // added by the code generator. Better don't move it.
