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

#include "AnimationFrameThumbnail.h"

#include "GameplaySystem/Framework/GameInstance.h"

#include <algorithm>

namespace Core
{
    AnimationFrameThumbnail::AnimationFrameThumbnail(
        const Animation::FrameByFrameAnimation& animation, std::size_t index)
        : _animation(animation),
          _index(index)
    {
        setFlex(Gui::Flex::Fixed);
        Layout::setWidth(112.f);
        Layout::setHeight(88.f);
        _unavailable.setText("Unavailable"_atom);
    }

    void AnimationFrameThumbnail::onInitialize()
    {
        Gui::VerticalLayout::onInitialize();
        _unavailable.initialize();
    }

    void AnimationFrameThumbnail::onDraw()
    {
        auto* assets = GetAssetsManager();
        if (!assets)
        {
            _unavailable.tick(ImGui::GetIO().DeltaTime);
            return;
        }
        const auto atlasNames = assets->getAtlasesAsVector();
        if (_index >= _animation.getFramesCount()
            || std::ranges::find(atlasNames, _animation.getAtlasName()) == atlasNames.end())
        {
            _unavailable.tick(ImGui::GetIO().DeltaTime);
            return;
        }
        const auto& atlas = GetAssetsManager()->getAtlas(_animation.getAtlasName());
        const auto& texture = atlas.getTexture();
        const auto& frame = _animation.getFrames()[_index];
        auto offset = glm::vec2(frame.uvOffset);
        auto size = glm::vec2(frame.uvSize);
        const auto regionName = frame.textureName.value_or(_animation.getTextureName());
        if (!regionName.isEmpty())
        {
            if (!atlas.getRects().contains(regionName))
            {
                _unavailable.tick(ImGui::GetIO().DeltaTime);
                return;
            }
            const auto& rect = atlas.getRect(regionName);
            const auto regionSize = rect.getRightBottom() - rect.getLeftTop();
            offset = rect.getLeftTop() + regionSize * offset;
            size *= regionSize;
        }
        const auto textureSize = texture.getSize();
        const auto pixels = size * glm::vec2(textureSize.width, textureSize.height);
        if (!texture.isValid() || pixels.x <= 0.f || pixels.y <= 0.f)
        {
            _unavailable.tick(ImGui::GetIO().DeltaTime);
            return;
        }
        const auto origin = ImGui::GetCursorScreenPos();
        const float scale = std::min(getWidth() / pixels.x, getHeight() / pixels.y);
        const auto imageSize = pixels * scale;
        const auto topLeft = origin + (glm::vec2(getWidth(), getHeight()) - imageSize) * 0.5f;
        auto* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(origin, origin + glm::vec2(getWidth(), getHeight()),
                                ImGui::GetColorU32(ImGuiCol_FrameBg));
        drawList->AddImage(const_cast<Texture&>(texture).getTextureId(), topLeft,
                           topLeft + imageSize, { offset.x, offset.y + size.y },
                           { offset.x + size.x, offset.y });
        ImGui::Dummy({ getWidth(), getHeight() });
    }

} // namespace Core
