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

#include "AnimationFramesPanel.h"

#include "AnimationFrameThumbnail.h"

namespace Core
{
    void AnimationFramesPanel::onInitialize()
    {
        Gui::VerticalLayout::onInitialize();
        _strip.setFitContent(true);
        _strip.setVerticalAlign(Gui::Align::Top);
        _empty.setText("Added frames will appear here"_atom);
        _empty.initialize();
    }

    void AnimationFramesPanel::onDraw()
    {
        if (!_draft)
        {
            return;
        }
        if (_displayedFrames != _draft->getFramesCount())
        {
            _strip.removeChildIf([](const BaseComponent*) { return true; });
            for (std::size_t i = 0; i < _draft->getFramesCount(); ++i)
            {
                auto* card = _strip.addChildComponent<Gui::VerticalLayout>();
                card->setFlex(Gui::Flex::Fixed);
                card->setWidth(112.f);
                card->setHorizontalAlign(Gui::Align::Left);
                card->addChildComponent<Gui::Label>()->setText("Frame "_atom
                                                               + StringAtom::MakeFrom(i + 1));
                card->addChildComponent<AnimationFrameThumbnail>(*_draft, i);
                auto* name = card->addChildComponent<Gui::Label>();
                name->setWidth(112.f);
                name->setText(_draft->getFrames()[i].name);
            }
            _displayedFrames = _draft->getFramesCount();
        }
        if (ImGui::BeginChild("AnimationFrames", { getWidth(), getHeight() }, true,
                              ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (_draft->hasFrames())
            {
                const auto origin = ImGui::GetCursorPos();
                _strip.tick(ImGui::GetIO().DeltaTime);
                // Layout cursor movement alone does not register content bounds with ImGui.
                ImGui::SetCursorPos(origin);
                ImGui::Dummy({ _strip.getWidth(), _strip.getHeight() });
            }
            else
            {
                _empty.tick(ImGui::GetIO().DeltaTime);
            }
        }
        ImGui::EndChild();
    }

} // namespace Core
