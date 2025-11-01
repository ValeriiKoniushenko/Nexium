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

#pragma once

#include "Editor/GuiComponents/Array.h"
#include "Editor/GuiComponents/Combo.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "EditorActorAdapter.h"
#include "Misc/Assimp.h"

namespace Core
{
    using AssimpPostProcessArray =
    Gui::BaseArray<aiPostProcessSteps,
        decltype([](aiPostProcessSteps data)
        -> Gui::HorizontalLayout::Ptr {
                auto l = Gui::HorizontalLayout::Create();
                const auto combo = l->addChildComponent<Gui::ComboModelBased>();

                combo->setDataProvider(
                    [](std::size_t i, StringAtom &out) -> const void * {
                        out = Assimp::aiPostProcessStepsToString(Assimp::aiPostProcessStepsAsVector[i]);
                        return nullptr;
                    });
                combo->setSizeProvider(
                    [] {
                        return Assimp::aiPostProcessStepsAsVector.size();
                    });
                combo->setFlex(Gui::Flex::FlexWidth);

                const auto it = std::ranges::find(Assimp::aiPostProcessStepsAsVector, data);
                if (it != Assimp::aiPostProcessStepsAsVector.end()) {
                    combo->setCurrentIndex(it - Assimp::aiPostProcessStepsAsVector.begin());
                }
                return l;
            }),
        decltype([](Gui::HorizontalLayout *layout)
        -> aiPostProcessSteps {
                if (auto modifier = layout->getFirstChildAs<Gui::ComboModelBased>()) {
                    return Assimp::aiPostProcessStepsAsVector[modifier->getCurrentIndex()];
                }
                else
                {
                    Assert(false);
                }

                return static_cast<aiPostProcessSteps>(0);
            })
    >;

    class ECSEditorStaticMeshBundleAdapter : public ECSEditorActorAdapter
    {
        ECS_COMPONENT_DECL(ECSEditorStaticMeshBundleAdapter, ECSEditorActorAdapter);

    protected:
        [[nodiscard]] std::size_t convertShaderNameToIndex(const StringAtom& shaderName) const;
        [[nodiscard]] StringAtom convertIndexToShaderName(std::size_t index) const;

        void onInitialize() override;
        void onApplyAssetData(const nlohmann::json& json) override;
        void onDraw(float dt) override;

    protected:
        Gui::VerticalLayout _layout;

        Gui::LabelRow<Gui::TextInput>* _logicalPath = nullptr;

        Gui::LabelRow<Gui::TextInput>* _modelInput = nullptr;
        Gui::LabelRow<Gui::ComboModelBased>* _mainShaderCombo = nullptr;
        Gui::LabelRow<Gui::ComboModelBased>* _outlineShaderCombo = nullptr;
        Gui::LabelRow<Gui::FloatInput>* _onLoadScale = nullptr;
        Gui::LabelRow<AssimpPostProcessArray>* _postProcessArray = nullptr;
        int _postProcessFlags = 0;
    };

} // namespace Core