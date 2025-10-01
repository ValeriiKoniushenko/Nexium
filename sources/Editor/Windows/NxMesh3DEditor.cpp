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

#include "NxMesh3DEditor.h"

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/Combo.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/List.h"
#include "Editor/GuiComponents/Separator.h"
#include "Editor/GuiComponents/Spacer.h"
#include "GameplaySystem/Framework/GameInstance.h"

using namespace Core::Gui;

namespace Core
{
    ECS_COMPONENT_IMPL(NxMesh3DEditorEWC)

    void NxMesh3DEditorEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        {
            auto* h = _layout.addChildComponent<HorizontalLayout>();

            auto* input = h->addChildComponent<TextInput>();
            input->setFlex(Gui::Flex::FlexWidth);
            input->setReadOnly(true);
            input->setPlaceholder("Press 'Open' to open some .nxmesh3d file");

            auto* button = h->addChildComponent<Button>("Open");
            button->onClick.subscribe(
                [input]
                {
                    const auto path = AssetsManager::OpenFileSelectionDialog({ "*.nxmesh3d" });
                    if (path.isEmpty())
                    {
                        input->setInputtedData("Error. Invalid file or path");
                    }
                    else
                    {
                        input->setInputtedData(path.toStdString());
                    }
                });
        }
    }

    void NxMesh3DEditorEWC::onDraw()
    {
        const float tick = gGameInstance->world.timeDelta;

        // if (ImGui::BeginChild("Properties", glm::vec2(200.f, 0), ImGuiChildFlags_ResizeX))
        {
            _layout.tick(tick);
        }
        // ImGui::EndChild();

        /*ImGui::SameLine();

        if (ImGui::BeginChild("View"))
        {
        }
        ImGui::EndChild();*/
    }

} // namespace Core