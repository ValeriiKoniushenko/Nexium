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

#include "AnimationPropertiesPanel.h"

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/Separator.h"
#include "GameplaySystem/Framework/GameInstance.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace Core
{
    void AnimationPropertiesPanel::onInitialize()
    {
        static constexpr float defaultLabelWidth = 50.0f;
        static constexpr float defaultLabelWidthBig = 150.0f;

        Gui::VerticalLayout::onInitialize();
        _fields.setHorizontalAlign(Gui::Align::Left);
        _fields.setSpacing(10.f);

        _fields.addChildComponent<Gui::Label>()->setText("Animation properties"_atom);
        _fields.addChildComponent<Gui::Separator>();

        {
            auto* h = _fields.addChildComponent<Gui::HorizontalLayout>();

            auto* label = h->addChildComponent<Gui::Label>();
            label->setText("Name"_atom);
            label->setWidth(defaultLabelWidth);

            _name = h->addChildComponent<Gui::TextInput>("Animation name"_atom);
            _name->setPlaceholder("New animation"_atom);
            _name->setFlex(Gui::Flex::FlexWidth);
        }
        {
            auto* h = _fields.addChildComponent<Gui::HorizontalLayout>();

            auto* label = h->addChildComponent<Gui::Label>();
            label->setText("Atlas"_atom);
            label->setWidth(defaultLabelWidth);

            _atlas = h->addChildComponent<Gui::ComboView>("Animation atlas"_atom);
            _atlas->setFlex(Gui::Flex::FlexWidth);
            _atlas->setData(std::vector<StringAtom>{ "Select atlas"_atom });

            _atlasHint = _fields.addChildComponent<Gui::Label>();
            _atlasHint->setText("No atlases loaded"_atom);
            _atlasHint->setTextColor(
                Color4::From(NormColor4(ImGui::GetStyle().Colors[ImGuiCol_TextDisabled])));
        }
        {
            auto* h = _fields.addChildComponent<Gui::HorizontalLayout>();

            auto* label = h->addChildComponent<Gui::Label>();
            label->setWidth(defaultLabelWidth);
            label->setText("FPS"_atom);
            _fps = h->addChildComponent<Gui::NumInput<float>>("Animation FPS"_atom);
            _fps->setFlex(Gui::Flex::FlexWidth);
            _fps->setInputtedData(10.f);
            _fps->setStep(0.25f);
            _fps->setMin(0.01f);
            _fps->setMax(1000.f);
        }
        {
            auto* h = _fields.addChildComponent<Gui::HorizontalLayout>();
            auto* label = h->addChildComponent<Gui::Label>();
            label->setWidth(defaultLabelWidth);
            label->setText("Loop"_atom);
            auto* checkBox = h->addChildComponent<Gui::CheckBox>();

            _loop = checkBox;
            _loop->setValue(true);
        }
        initializeFrameSource();

        auto* add = _fields.addChildComponent<Gui::Button>("Add animation frame"_atom);
        add->setText("+ Add frame"_atom);
        add->setFlex(Gui::Flex::FlexWidth);
        _addFrameSubscription = add->onClick->subscribeAndGetID([this] { addFrame(); });
        _frameStatus = _fields.addChildComponent<Gui::Label>();
        _frameStatus->setFlex(Gui::Flex::FlexWidth);
        _frameStatus->setText("Select a source to add frames"_atom);
    }

    std::optional<Animation::Frame> AnimationPropertiesPanel::frameFromPixelRect(
        glm::ivec4 rect, glm::ivec2 textureSize)
    {
        const auto [x, y, width, height] = std::array{ rect.x, rect.y, rect.z, rect.w };
        if (textureSize.x <= 0 || textureSize.y <= 0 || x < 0 || y < 0 || width <= 0 || height <= 0
            || width > textureSize.x || height > textureSize.y || x > textureSize.x - width
            || y > textureSize.y - height)
        {
            return std::nullopt;
        }
        const auto size = glm::vec2(textureSize);
        Animation::Frame frame;
        frame.uvOffset
            = GlobalPosition2F{ static_cast<float>(x) / size.x,
                                static_cast<float>(textureSize.y - y - height) / size.y };
        frame.uvSize = GlobalPosition2F{
            std::min(static_cast<float>(width) / size.x, 1.f - frame.uvOffset.x),
            std::min(static_cast<float>(height) / size.y, 1.f - frame.uvOffset.y)
        };
        return frame;
    }

    void AnimationPropertiesPanel::addFrame()
    {
        if (!_draft)
        {
            return;
        }
        const auto atlasNames = GetAssetsManager()->getAtlasesAsVector();
        const auto atlasName = _atlas->getSelectedString();
        if (_atlas->getCurrentIndex() == 0
            || std::ranges::find(atlasNames, atlasName) == atlasNames.end())
        {
            _frameStatus->setText("Select a loaded atlas"_atom);
            return;
        }
        if (_draft->hasFrames() && _draft->getAtlasName() != atlasName)
        {
            _frameStatus->setText("Frames must use the same atlas"_atom);
            return;
        }
        const auto& atlas = GetAssetsManager()->getAtlas(atlasName);
        bool added = false;
        if (_frameSource->getCurrentIndex() == 0)
        {
            if (_regionAtlas != atlasName || _region->getCurrentIndex() == 0
                || !atlas.getRects().contains(_region->getSelectedString()))
            {
                _frameStatus->setText("Select a valid atlas region"_atom);
                return;
            }
            added = _draft->addFrame(_region->getSelectedString());
        }
        else
        {
            const auto& texture = atlas.getTexture();
            const auto size = texture.getSize();
            const auto frame = frameFromPixelRect(
                { _rectX->getInputtedData(), _rectY->getInputtedData(),
                  _rectWidth->getInputtedData(), _rectHeight->getInputtedData() },
                { size.width, size.height });
            if (!texture.isValid() || !frame)
            {
                _frameStatus->setText("Rectangle must fit the texture"_atom);
                return;
            }
            added = _draft->addFrame(frame->uvOffset, frame->uvSize);
        }
        if (added)
        {
            _draft->setAtlasName(atlasName);
            _draft->setTextureName({});
            _frameStatus->setText("Frame added"_atom);
        }
        else
        {
            _frameStatus->setText("Could not add this frame"_atom);
        }
    }

    void AnimationPropertiesPanel::initializeFrameSource()
    {
        _fields.addChildComponent<Gui::Separator>();
        _fields.addChildComponent<Gui::Label>()->setText("Frame source"_atom);
        _frameSource = _fields.addChildComponent<Gui::ComboView>("Frame source"_atom);
        _frameSource->setFlex(Gui::Flex::FlexWidth);
        _frameSource->setData(
            std::vector<StringAtom>{ "Named atlas region"_atom, "Sprite sheet rectangle"_atom });

        _regionFields = _fields.addChildComponent<Gui::VerticalLayout>("Atlas regions"_atom);
        _regionFields->setHorizontalAlign(Gui::Align::Left);
        _regionFields->addChildComponent<Gui::Label>()->setText("Region"_atom);
        _region = _regionFields->addChildComponent<Gui::ComboView>("Region"_atom);
        _region->setFlex(Gui::Flex::FlexWidth);
        _region->setData(std::vector<StringAtom>{ "Select atlas first"_atom });

        _rectFields = _fields.addChildComponent<Gui::VerticalLayout>("Frame rectangle"_atom);
        _rectFields->setHorizontalAlign(Gui::Align::Left);
        _rectFields->addChildComponent<Gui::Label>()->setText("Rectangle in pixels"_atom);
        _rectFields->addChildComponent<Gui::Label>()->setText("Origin: top-left of texture"_atom);

        const auto addCoordinate = [this](const StringAtom& label, int minimum, int initial)
        {
            auto* row
                = _rectFields->addChildComponent<Gui::LabelRow<Gui::NumInput<int>>>(label, 70.f);
            row->input->setFlex(Gui::Flex::FlexWidth);
            row->input->setMin(minimum);
            row->input->setStep(1);
            row->input->setInputtedData(initial);
            return row->input;
        };
        _rectX = addCoordinate("X"_atom, 0, 0);
        _rectY = addCoordinate("Y"_atom, 0, 0);
        _rectWidth = addCoordinate("Width"_atom, 1, 32);
        _rectHeight = addCoordinate("Height"_atom, 1, 32);
        _rectFields->setEnabled(false);
    }

    void AnimationPropertiesPanel::updateFrameSource()
    {
        const bool namedRegion = _frameSource->getCurrentIndex() == 0;
        _regionFields->setEnabled(namedRegion);
        _rectFields->setEnabled(!namedRegion);

        const bool hasAtlas = _atlas->getCurrentIndex() != 0;
        const auto atlasName = hasAtlas ? _atlas->getSelectedString() : StringAtom{};
        auto regions = hasAtlas ? GetAssetsManager()->getAtlas(atlasName).getRectsAsVector()
                                : std::vector<StringAtom>{};
        const bool hasRegions = !regions.empty();
        regions.insert(regions.begin(), !hasAtlas    ? "Select atlas first"_atom
                                        : hasRegions ? "Select region"_atom
                                                     : "No named regions"_atom);
        if (atlasName != _regionAtlas || regions != _region->getData())
        {
            const auto previous = _region->getSelectedString();
            const auto found = std::ranges::find(regions, previous);
            const auto index = atlasName == _regionAtlas && found != regions.end()
                                   ? static_cast<std::size_t>(found - regions.begin())
                                   : 0U;
            _region->setData(std::move(regions));
            _region->setCurrentIndex(index);
            _regionAtlas = atlasName;
        }
        _region->disableWidget(!hasRegions);
        _rectX->setInputtedData(std::max(0, _rectX->getInputtedData()));
        _rectY->setInputtedData(std::max(0, _rectY->getInputtedData()));
        _rectWidth->setInputtedData(std::max(1, _rectWidth->getInputtedData()));
        _rectHeight->setInputtedData(std::max(1, _rectHeight->getInputtedData()));
    }

    void AnimationPropertiesPanel::onDraw()
    {
        if (ImGui::BeginChild("PropertiesPanel", { getWidth(), getHeight() }, true))
        {
            auto atlasNames = GetAssetsManager()->getAtlasesAsVector();
            _atlasHint->setEnabled(atlasNames.empty());
            atlasNames.insert(atlasNames.begin(), "Select atlas"_atom);
            if (atlasNames != _atlas->getData())
            {
                const auto selected = _atlas->getSelectedString();
                const auto found = std::ranges::find(atlasNames, selected);
                const auto index = found == atlasNames.end()
                                       ? 0U
                                       : static_cast<std::size_t>(found - atlasNames.begin());
                _atlas->setData(std::move(atlasNames));
                _atlas->setCurrentIndex(index);
            }
            _atlas->disableWidget(_draft && _draft->hasFrames());
            updateFrameSource();
            _fields.tick(ImGui::GetIO().DeltaTime);
            const float fps = _fps->getInputtedData();
            _fps->setInputtedData(std::isfinite(fps) ? std::clamp(fps, 0.01f, 1000.f) : 10.f);
            if (_draft)
            {
                _draft->setComponentName(StringAtom{ _name->getInputtedData() });
                _draft->setFPS(_fps->getInputtedData());
                _draft->setLoop(_loop->getValue());
            }
        }
        ImGui::EndChild();
    }

} // namespace Core
