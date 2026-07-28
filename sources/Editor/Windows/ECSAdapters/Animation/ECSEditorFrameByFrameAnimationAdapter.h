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
#include "Animations/FrameByFrame/FrameByFrameAnimation.h"
#include "Core/Delegate.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "Editor/Windows/NxECSBasedEditor.h"

#include <array>
#include <unordered_map>

namespace Core
{
    namespace Gui
    {
        class Button;
    }

    CLASS();
    class ECSEditorFrameByFrameAnimationAdapter : public ECSEditorMimeAdapter
    {
        ECS_DECL(ECSEditorFrameByFrameAnimationAdapter, Core::ECSEditorMimeAdapter);

    public:
        [[nodiscard]] bool canWorkWith(BaseComponent* component) const override;
        [[nodiscard]] StringAtom getProcessedAssetType() const override;

    protected:
        void onInitialize() override;
        void onDraw(float dt) override;
        void onApplyAssetData(const nlohmann::json&) override {}

    private:
        void beginCreate();
        void beginEdit(const StringAtom& name);
        void drawAnimationCards();
        void drawAnimationEditor();
        void drawPreview(const Animation::FrameByFrameAnimation& animation, float dt,
                         float size, const StringAtom& previewId);
        void copyDraftToBuffers();
        void applyBuffersToDraft();

        Gui::VerticalLayout _layout;
        Gui::Button* _addAnimButton = nullptr;
        DelegateSubscriberPoolGuard _subscriptionPool;

        Animation::FrameByFrameAnimation _draft;
        StringAtom _editedName;
        bool _showEditor = false;
        bool _isCreating = false;
        std::array<char, 128> _name{};
        std::array<char, 256> _atlas{};
        std::array<char, 256> _texture{};
        std::array<char, 256> _frameTexture{};
        float _fps = 10.f;
        bool _loop = true;
        int _sheetColumns = 1;
        int _sheetRows = 1;
        int _sheetSelectedRow = 1;
        struct PreviewState
        {
            std::size_t frame = 0;
            float timer = 0.f;
        };
        std::unordered_map<StringAtom, PreviewState> _previewStates;
    };
}

#include "ECSEditorFrameByFrameAnimationAdapter.generated.h"
