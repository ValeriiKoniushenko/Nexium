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

#include "BaseWindow.h"
#include "Editor/GuiComponents/VerticalLayout.h"

namespace Core
{
    namespace Gui
    {
        template<Utils::IsArithmetic T>
        class NumInput;

        using IntInput = NumInput<int>;
        using FloatInput = NumInput<float>;

        class Color3Input;

        template<std::size_t Size, Utils::IsArithmetic Type>
        class VecNumInput;

        using Float3Input = VecNumInput<3, float>;
    } // namespace Gui

    CLASS();
    class WorldLightningEWC : public BaseFloatEWC
    {
        R_FRIEND_DECL(Core::WorldLightningEWC, Core::BaseFloatEWC);
        ECS_COMPONENT_DECL(WorldLightningEWC, BaseFloatEWC);

    public:
        [[nodiscard]] const char* getIcon() override;

    protected:
        void onInitialize() override;
        void onOpen() override;
        void onUpdate() override;

        void onDraw() override;

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;
        Gui::VerticalLayout _layout;

        Gui::Color3Input* _color3Input = nullptr;
        Gui::FloatInput* _ambientStrength = nullptr;
        Gui::FloatInput* _minLightStrength = nullptr;
        Gui::FloatInput* _specularStrength = nullptr;
        Gui::FloatInput* _specularPow = nullptr;
        Gui::Float3Input* _sunDirection = nullptr;
    };
} // namespace Core

#include "WorldLightning.generated.h" // added by the code generator. Better don't move it.
