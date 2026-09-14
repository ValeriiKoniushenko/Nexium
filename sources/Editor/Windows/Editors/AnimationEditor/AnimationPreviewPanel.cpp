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

#include "AnimationPreviewPanel.h"

#include "AnimationFrameThumbnail.h"

#include <algorithm>

namespace NX
{
    void AnimationPreviewPanel::setDraft(const Animation::FrameByFrameAnimation& draft)
    {
        _draft = &draft;
        _preview = draft;
        _preview.restart();
    }

    void AnimationPreviewPanel::synchronizeDraft()
    {
        if (!_draft)
        {
            return;
        }
        if (_preview.getAtlasName() != _draft->getAtlasName())
        {
            _preview = *_draft;
            _preview.restart();
            return;
        }
        _preview.setComponentName(_draft->getComponentName());
        _preview.setAtlasName(_draft->getAtlasName());
        _preview.setTextureName(_draft->getTextureName());
        _preview.setFPS(_draft->getFPS());
        _preview.setLoop(_draft->isLooping());

        // Update frame data without replacing the animation's playback state.
        while (_preview.getFramesCount() > _draft->getFramesCount())
        {
            _preview.removeFrame(_preview.getFramesCount() - 1);
        }
        for (std::size_t i = 0; i < _draft->getFramesCount(); ++i)
        {
            if (i == _preview.getFramesCount())
            {
                _preview.addFrame(GlobalPosition2F{ 0.f, 0.f }, GlobalPosition2F{ 1.f, 1.f });
            }
            _preview.setFrame(i, _draft->getFrames()[i]);
        }
    }

    std::size_t AnimationPreviewPanel::currentFrameIndex() const
    {
        const auto* frame = _preview.getCurrentFrame();
        return frame ? static_cast<std::size_t>(frame - _preview.getFrames().data()) : 0U;
    }

    void AnimationPreviewPanel::drawCanvas(glm::vec2 size)
    {
        const auto origin = ImGui::GetCursorScreenPos();
        auto* drawList = ImGui::GetWindowDrawList();
        constexpr float cellSize = 16.f;
        const auto dark = ImGui::GetColorU32(ImGuiCol_WindowBg);
        const auto light = ImGui::GetColorU32(ImGuiCol_ChildBg);

        for (int row = 0; static_cast<float>(row) * cellSize < size.y; ++row)
        {
            for (int column = 0; static_cast<float>(column) * cellSize < size.x; ++column)
            {
                const float x = static_cast<float>(column) * cellSize;
                const float y = static_cast<float>(row) * cellSize;
                drawList->AddRectFilled({ origin.x + x, origin.y + y },
                                        { origin.x + std::min(x + cellSize, size.x),
                                          origin.y + std::min(y + cellSize, size.y) },
                                        (row + column) % 2 == 0 ? dark : light);
            }
        }

        const bool hasFrames = _preview.hasFrames();
        if (!hasFrames
            || !AnimationFrameThumbnail::drawFrame(_preview, currentFrameIndex(), origin, size))
        {
            const auto message = hasFrames ? "Preview unavailable: check atlas"_atom
                                           : "Select an atlas to preview your animation"_atom;
            const auto textSize = ImGui::CalcTextSize(message.c_str());
            drawList->AddText({ origin.x + std::max(0.f, (size.x - textSize.x) * 0.5f),
                                origin.y + std::max(0.f, (size.y - textSize.y) * 0.5f) },
                              ImGui::GetColorU32(ImGuiCol_TextDisabled), message.c_str());
        }
        ImGui::Dummy(size);
    }

    void AnimationPreviewPanel::onInitialize()
    {
        Gui::VerticalLayout::onInitialize();
        _title.setText("Animation preview"_atom);
        _title.initialize();
        _controls.setFitContent(true);
        _controls.setVerticalAlign(Gui::Align::Center);
        _play = _controls.addChildComponent<Gui::Button>("Preview playback"_atom);
        _play->setText("Pause"_atom);
        _play->setSize({ 76.f, 26.f });
        _restart = _controls.addChildComponent<Gui::Button>("Restart preview"_atom);
        _restart->setText("Restart"_atom);
        _restart->setSize({ 76.f, 26.f });
        _position = _controls.addChildComponent<Gui::Label>();
        _subscriptions << _play->onClick->subscribeAndGetID(
            [this]
            {
                if (_preview.isPlaying())
                {
                    _preview.pause();
                }
                else if (_preview.isPaused())
                {
                    _preview.resume();
                }
                else
                {
                    _preview.start();
                }
            });
        _subscriptions << _restart->onClick->subscribeAndGetID([this] { _preview.restart(); });
    }

    void AnimationPreviewPanel::onDraw()
    {
        if (ImGui::BeginChild("PreviewPanel", { getWidth(), getHeight() }, true))
        {
            _title.tick(ImGui::GetIO().DeltaTime);
            synchronizeDraft();
            _preview.tick(ImGui::GetIO().DeltaTime);
            const auto count = _preview.getFramesCount();
            _play->disableWidget(count == 0);
            _restart->disableWidget(count == 0);
            _play->setText(_preview.isPlaying() ? "Pause"_atom : "Play"_atom);
            _position->setText(count == 0 ? "No frames"_atom
                                          : StringAtom::MakeFrom(currentFrameIndex() + 1)
                                                + " / "_atom + StringAtom::MakeFrom(count));
            const auto available = ImGui::GetContentRegionAvail();
            drawCanvas({ std::max(available.x, 1.f),
                         std::max(1.f, available.y - _controls.getHeight()
                                           - ImGui::GetStyle().ItemSpacing.y) });
            const auto origin = ImGui::GetCursorPos();
            _controls.tick(ImGui::GetIO().DeltaTime);
            ImGui::SetCursorPos(origin);
            ImGui::Dummy({ _controls.getWidth(), _controls.getHeight() });
        }
        ImGui::EndChild();
    }

} // namespace NX
