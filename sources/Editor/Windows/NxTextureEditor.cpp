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

#include "NxTextureEditor.h"

#include "Editor/GuiComponents/Combo.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/List.h"
#include "GameplaySystem/Framework/GameInstance.h"

#include <iterator>

using namespace Core::Gui;

namespace Core
{
    ECS_COMPONENT_IMPL(NxTextureEditorEWC)

    void NxTextureEditorEWC::onInitialize()
    {
        NxEditorBaseEditorEWC::onInitialize();

        _fileFilters.emplace(std::string("*") + NXTexture::AssetT::fileExtension);

        constexpr float defaultLabelWidth = 140.0f;
        constexpr float defaultModifierWidth = 340.0f;
    }

    void NxTextureEditorEWC::onDraw()
    {
        NxEditorBaseEditorEWC::onDraw();

        if (Gui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _layout.tick(GetWorld().timeDelta);
        }
    }

    void NxTextureEditorEWC::onDiscardChanges()
    {
        NxEditorBaseEditorEWC::onDiscardChanges();

        _targetAsset
            = GetAssetsManager().getTexture(StringAtom::Intern(_assetFilePath.generic_string()));
    }

    void NxTextureEditorEWC::onSave()
    {
        NxEditorBaseEditorEWC::onSave();

        if (!_targetAsset.isValid())
        {
            return;
        }

        _targetAsset->writeToFile();

        _targetAsset->makeHotReload();
    }

    void NxTextureEditorEWC::onOpenFromPath(const std::filesystem::path& path)
    {
        _targetAsset = GetAssetsManager().getTexture(StringAtom::Intern(path.generic_string()));
    }

} // namespace Core