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

#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Separator.h"
#include "NxWorld/Framework/GameInstance.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace NX
{
    void AnimationPropertiesPanel::onInitialize()
    {
        static constexpr float defaultLabelWidth = 50.0f;

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
        _frameStatus = _fields.addChildComponent<Gui::Label>();
        _frameStatus->setFlex(Gui::Flex::FlexWidth);
        _frameStatus->setText("Select an atlas to load all frames"_atom);
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

    void AnimationPropertiesPanel::setAtlasFrames(const StringAtom& atlasName,
                                                  std::vector<StringAtom> regions)
    {
        if (!_draft)
        {
            return;
        }
        std::ranges::sort(regions);
        _draft->clearFrames();
        _draft->setAtlasName(atlasName);
        _draft->setTextureName({});
        for (const auto& region : regions)
        {
            _draft->addFrame(region);
        }
    }

    void AnimationPropertiesPanel::updateAtlas()
    {
        const auto atlasName
            = _atlas->getCurrentIndex() == 0 ? StringAtom{} : _atlas->getSelectedString();
        if (!_draft || atlasName == _selectedAtlas)
        {
            return;
        }
        setAtlasFrames(atlasName, atlasName
                                      ? GetAssetsManager()->getAtlas(atlasName).getRectsAsVector()
                                      : std::vector<StringAtom>{});
        _selectedAtlas = atlasName;
        _frameStatus->setText(!atlasName            ? "Select an atlas to load all frames"_atom
                              : _draft->hasFrames() ? "All atlas regions loaded"_atom
                                                    : "Atlas has no regions"_atom);
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
            _fields.tick(ImGui::GetIO().DeltaTime);
            updateAtlas();
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

} // namespace NX
