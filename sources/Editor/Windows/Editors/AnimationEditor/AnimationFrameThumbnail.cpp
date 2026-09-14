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

#include "NxWorld/Framework/GameInstance.h"
#include "ImGui/imgui.h"

#include <algorithm>

namespace NX
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
        _drawList = ImGui::GetWindowDrawList();
    }

    void AnimationFrameThumbnail::onDraw()
    {
        const auto thumbnailPos = ImGui::GetCursorScreenPos();
        const auto thumbnailSize = glm::vec2(getWidth(), getHeight());

        ImGui::PushID(this);

        ImGui::InvisibleButton("##thumbnail", { thumbnailSize.x, thumbnailSize.y });

        const auto nextThumbnailPos = ImGui::GetCursorScreenPos();

        const auto background = ImGui::IsItemActive()    ? ImGuiCol_ButtonActive
                                : ImGui::IsItemHovered() ? ImGuiCol_ButtonHovered
                                                         : ImGuiCol_FrameBg;

        _drawList->AddRectFilled(thumbnailPos, thumbnailPos + thumbnailSize,
                                 ImGui::GetColorU32(ImGuiCol_FrameBg));

        if (!drawFrame(_animation, _index, thumbnailPos, thumbnailSize))
        {
            ImGui::SetCursorScreenPos(thumbnailPos);
            _unavailable.tick(ImGui::GetIO().DeltaTime);
        }

        if (ImGui::IsItemHovered())
        {
            //draw borders
            _drawList->AddRect(thumbnailPos, thumbnailPos + thumbnailSize,
                               ImGui::GetColorU32(ImGuiCol_ButtonHovered));
        }

        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            infoLog("frame clicked");
        }

        ImGui::SetCursorScreenPos(nextThumbnailPos);
        ImGui::PopID();
    }

    bool AnimationFrameThumbnail::drawFrame(const Animation::FrameByFrameAnimation& animation,
                                            std::size_t index, glm::vec2 origin, glm::vec2 bounds)
    {
        auto* assets = GetAssetsManager();
        if (!assets)
        {
            return false;
        }
        const auto atlasNames = assets->getAtlasesAsVector();
        if (index >= animation.getFramesCount()
            || std::ranges::find(atlasNames, animation.getAtlasName()) == atlasNames.end())
        {
            return false;
        }
        const auto& atlas = GetAssetsManager()->getAtlas(animation.getAtlasName());
        const auto& texture = atlas.getTexture();
        const auto& frame = animation.getFrames()[index];
        auto offset = glm::vec2(frame.uvOffset);
        auto size = glm::vec2(frame.uvSize);
        const auto regionName = frame.textureName.value_or(animation.getTextureName());
        if (!regionName.isEmpty())
        {
            if (!atlas.getRects().contains(regionName))
            {
                return false;
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
            return false;
        }
        const float scale = std::min(bounds.x / pixels.x, bounds.y / pixels.y);
        const auto imageSize = pixels * scale;
        const auto topLeft = origin + (glm::vec2(bounds.x, bounds.y) - imageSize) * 0.5f;
        auto* drawList = ImGui::GetWindowDrawList();
        drawList->AddImage(const_cast<RawBackend::Texture&>(texture).getTextureId(), topLeft,
                           topLeft + imageSize, { offset.x, offset.y + size.y },
                           { offset.x + size.x, offset.y });
        return true;
    }

} // namespace NX
