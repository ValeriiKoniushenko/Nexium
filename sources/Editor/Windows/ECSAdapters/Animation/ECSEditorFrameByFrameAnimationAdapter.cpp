/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 */

#include "ECSEditorFrameByFrameAnimationAdapter.h"

#include "Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/Misc.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Misc/IconsFontAwesome.h"

#include <algorithm>
#include <cstring>

namespace
{
    template<std::size_t Size>
    void CopyToBuffer(std::array<char, Size>& destination, const Core::StringAtom& source)
    {
        destination.fill('\0');
        const auto& text = source.toStdString();
        std::memcpy(destination.data(), text.data(), std::min(text.size(), Size - 1));
    }
}

namespace Core
{
    ECS_IMPL(ECSEditorFrameByFrameAnimationAdapter);

    StringAtom ECSEditorFrameByFrameAnimationAdapter::getProcessedAssetType() const
    {
        return Animation::FrameByFrameAnimator::componentType;
    }

    bool ECSEditorFrameByFrameAnimationAdapter::canWorkWith(BaseComponent* component) const
    {
        return dynamic_cast<Animation::FrameByFrameAnimator*>(component) != nullptr;
    }

    void ECSEditorFrameByFrameAnimationAdapter::onInitialize()
    {
        ECSEditorMimeAdapter::onInitialize();
        _layout.setPaddings(glm::vec4{ImGui::GetStyle().ItemSpacing.x});
        _addAnimButton = _layout.addChildComponent<Gui::Button>();
        _addAnimButton->setText(ICON_FA_PLUS " Add animation");
        _addAnimButton->setFlex(Gui::Flex::FlexWidth);
        _subscriptionPool << _addAnimButton->onClick->subscribeAndGetID([this] { beginCreate(); });
    }

    void ECSEditorFrameByFrameAnimationAdapter::beginCreate()
    {
        _draft = Animation::FrameByFrameAnimation{};
        _draft.setLoop(true);
        _editedName.clear();
        _isCreating = true;
        _showEditor = true;
        copyDraftToBuffers();
        std::strncpy(_name.data(), "New animation", _name.size() - 1);
    }

    void ECSEditorFrameByFrameAnimationAdapter::beginEdit(const StringAtom& name)
    {
        auto* animator = dynamic_cast<Animation::FrameByFrameAnimator*>(getTargetComponent());
        if (!animator)
        {
            return;
        }
        if (const auto* animation = animator->getAnimation(name))
        {
            _draft = *animation;
            _editedName = name;
            _isCreating = false;
            _showEditor = true;
            copyDraftToBuffers();
        }
    }

    void ECSEditorFrameByFrameAnimationAdapter::copyDraftToBuffers()
    {
        CopyToBuffer(_name, _draft.getAnimationName());
        CopyToBuffer(_atlas, _draft.getAtlasName());
        CopyToBuffer(_texture, _draft.getTextureName());
        _frameTexture.fill('\0');
        _fps = _draft.getFPS();
        _loop = _draft.isLooping();
        _previewStates["editor"_atom] = {};
    }

    void ECSEditorFrameByFrameAnimationAdapter::applyBuffersToDraft()
    {
        _draft.setAnimationName(StringAtom{_name.data()});
        _draft.setAtlasName(StringAtom{_atlas.data()});
        _draft.setTextureName(StringAtom{_texture.data()});
        _draft.setFPS(std::max(_fps, 0.01f));
        _draft.setLoop(_loop);
    }

    void ECSEditorFrameByFrameAnimationAdapter::drawPreview(
        const Animation::FrameByFrameAnimation& animation, float dt, float size,
        const StringAtom& previewId)
    {
        const auto& frames = animation.getFrames();
        if (frames.empty())
        {
            ImGui::TextDisabled("Preview: no frames");
            return;
        }

        auto& preview = _previewStates[previewId];
        preview.frame %= frames.size();
        preview.timer += dt;
        const float frameTime = 1.f / std::max(animation.getFPS(), 0.01f);
        while (preview.timer >= frameTime)
        {
            preview.timer -= frameTime;
            preview.frame = (preview.frame + 1) % frames.size();
        }

        const auto atlasNames = GetAssetsManager().getAtlasesAsVector();
        if (std::ranges::find(atlasNames, animation.getAtlasName()) == atlasNames.end())
        {
            ImGui::TextDisabled("Preview unavailable (atlas '%s' is not loaded)",
                                animation.getAtlasName().c_str());
            return;
        }

        const auto& atlas = GetAssetsManager().getAtlas(animation.getAtlasName());
        const auto& frame = frames[preview.frame];
        glm::vec2 uv0{frame.uvOffset};
        glm::vec2 uv1 = uv0 + glm::vec2{frame.uvSize};

        const StringAtom regionName
            = frame.textureName.value_or(animation.getTextureName());
        if (!regionName.isEmpty() && atlas.getRects().contains(regionName))
        {
            const auto rect = atlas.getRect(regionName);
            const auto regionSize = rect.getRightBottom() - rect.getLeftTop();
            uv0 = rect.getLeftTop() + regionSize * frame.uvOffset;
            uv1 = rect.getLeftTop() + regionSize * (frame.uvOffset + frame.uvSize);
        }

        auto& texture = const_cast<Texture&>(atlas.getTexture());
        if (texture.isValid())
        {
            ImGui::Image(texture.getTextureId(), {size, size},
                         glm::vec2{uv0.x, uv1.y}, glm::vec2{uv1.x, uv0.y});
            ImGui::SameLine();
        }
        ImGui::Text("%zu / %zu", preview.frame + 1, frames.size());
    }

    void ECSEditorFrameByFrameAnimationAdapter::drawAnimationCards()
    {
        auto* animator = dynamic_cast<Animation::FrameByFrameAnimator*>(getTargetComponent());
        if (!animator)
        {
            return;
        }

        std::vector<StringAtom> names;
        names.reserve(animator->getAnimations().size());
        for (const auto& [name, animation] : animator->getAnimations())
        {
            names.push_back(name);
        }
        std::ranges::sort(names,
                          [](const auto& a, const auto& b)
                          { return a.toStdString() < b.toStdString(); });

        for (const auto& name : names)
        {
            const auto* animation = animator->getAnimation(name);
            if (!animation)
            {
                continue;
            }

            ImGui::PushID(name.c_str());
            ImGui::BeginGroup();
            ImGui::TextUnformatted(name.c_str());
            ImGui::SameLine();
            ImGui::TextDisabled("%zu frames | %.1f FPS%s", animation->getFramesCount(),
                                animation->getFPS(), animation->isLooping() ? " | loop" : "");
            drawPreview(*animation, ImGui::GetIO().DeltaTime, 112.f,
                        "card:"_atom + name);
            if (ImGui::Button(ICON_FA_PENCIL " Edit"))
            {
                beginEdit(name);
            }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_TRASH " Delete"))
            {
                animator->removeAnimation(name);
                makeParentDirty();
            }
            ImGui::EndGroup();
            ImGui::Separator();
            ImGui::PopID();
        }
    }

    void ECSEditorFrameByFrameAnimationAdapter::drawAnimationEditor()
    {
        if (!_showEditor)
        {
            return;
        }

        ImGui::OpenPopup("Frame-by-frame animation editor");
        if (!ImGui::BeginPopupModal("Frame-by-frame animation editor", &_showEditor,
                                    ImGuiWindowFlags_AlwaysAutoResize))
        {
            return;
        }

        ImGui::InputText("Name", _name.data(), _name.size());
        const auto atlasNames = GetAssetsManager().getAtlasesAsVector();
        const char* atlasPreview = _atlas[0] != '\0' ? _atlas.data() : "Select atlas";
        if (ImGui::BeginCombo("Atlas", atlasPreview))
        {
            for (const auto& atlasName : atlasNames)
            {
                const bool isSelected = atlasName == StringAtom{_atlas.data()};
                if (ImGui::Selectable(atlasName.c_str(), isSelected))
                {
                    CopyToBuffer(_atlas, atlasName);
                    _texture.fill('\0');
                    _frameTexture.fill('\0');
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            if (atlasNames.empty())
            {
                ImGui::TextDisabled("No atlases available");
            }
            ImGui::EndCombo();
        }
        std::vector<StringAtom> regionNames;
        if (_atlas[0] != '\0'
            && std::ranges::find(atlasNames, StringAtom{_atlas.data()}) != atlasNames.end())
        {
            regionNames = GetAssetsManager().getAtlas(StringAtom{_atlas.data()})
                              .getRectsAsVector();
        }

        if (!regionNames.empty())
        {
            const char* baseRegionPreview
                = _texture[0] != '\0' ? _texture.data() : "Select base region";
            if (ImGui::BeginCombo("UV base region", baseRegionPreview))
            {
                for (const auto& regionName : regionNames)
                {
                    const bool isSelected = regionName == StringAtom{_texture.data()};
                    if (ImGui::Selectable(regionName.c_str(), isSelected))
                    {
                        CopyToBuffer(_texture, regionName);
                    }
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::TextDisabled("UV frames are relative to this atlas region.");
        }
        else
        {
            _texture.fill('\0');
            ImGui::TextDisabled("This atlas has no named regions; UV uses the whole texture.");
        }
        ImGui::DragFloat("FPS", &_fps, 0.25f, 0.01f, 1000.f, "%.2f");
        ImGui::Checkbox("Loop", &_loop);
        applyBuffersToDraft();
        drawPreview(_draft, ImGui::GetIO().DeltaTime, 180.f, "editor"_atom);

        if (ImGui::CollapsingHeader("Frames", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::TextDisabled("%zu frame(s)", _draft.getFramesCount());
            ImGui::BeginChild("AnimationFrames", {520.f, 260.f}, true,
                              ImGuiWindowFlags_AlwaysVerticalScrollbar);
            std::optional<std::size_t> frameToDelete;
            for (std::size_t i = 0; i < _draft.getFramesCount(); ++i)
            {
                const auto frame = _draft.getFrames()[i];
                auto uvOffset = frame.uvOffset;
                auto uvSize = frame.uvSize;
                std::array<char, 128> frameName{};
                CopyToBuffer(frameName, frame.name);

                ImGui::PushID(static_cast<int>(i));
                ImGui::TextUnformatted(frame.name.isEmpty()
                                           ? ("Frame "_atom + StringAtom::MakeFrom(i + 1)).c_str()
                                           : frame.name.c_str());
                ImGui::SameLine();
                if (ImGui::SmallButton(ICON_FA_TRASH))
                {
                    frameToDelete = i;
                }
                auto edited = frame;
                if (ImGui::InputText("Name", frameName.data(), frameName.size()))
                {
                    edited.name = StringAtom{frameName.data()};
                }
                ImGui::DragFloat2("UV offset", &uvOffset.x, 0.005f, 0.f, 1.f);
                ImGui::DragFloat2("UV size", &uvSize.x, 0.005f, 0.001f, 1.f);
                edited.uvOffset = uvOffset;
                edited.uvSize = uvSize;
                _draft.setFrame(i, std::move(edited));
                ImGui::Separator();
                ImGui::PopID();
            }
            if (frameToDelete)
            {
                _draft.removeFrame(*frameToDelete);
            }
            ImGui::EndChild();
        }

        if (ImGui::CollapsingHeader("Named atlas frames", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (!regionNames.empty())
            {
                const char* frameRegionPreview
                    = _frameTexture[0] != '\0' ? _frameTexture.data() : "Select region";
                if (ImGui::BeginCombo("Atlas region", frameRegionPreview))
                {
                    for (const auto& regionName : regionNames)
                    {
                        const bool isSelected = regionName == StringAtom{_frameTexture.data()};
                        if (ImGui::Selectable(regionName.c_str(), isSelected))
                        {
                            CopyToBuffer(_frameTexture, regionName);
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::BeginDisabled(_frameTexture[0] == '\0');
                if (ImGui::Button("Add selected region"))
                {
                    _draft.addFrame(StringAtom{_frameTexture.data()});
                }
                ImGui::EndDisabled();
            }
            else
            {
                ImGui::TextDisabled("The selected atlas has no named regions.");
            }
        }

        if (ImGui::CollapsingHeader("Sprite sheet / UV frames",
                                    ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::TextDisabled(
                "Import one complete row from a uniformly spaced sprite sheet.");
            if (ImGui::Button("Add full-size UV frame"))
            {
                _draft.addFrame(GlobalPosition2F{0.f, 0.f}, GlobalPosition2F{1.f, 1.f});
            }

            ImGui::SetNextItemWidth(80.f);
            ImGui::InputInt("Frames in row", &_sheetColumns);
            ImGui::SetNextItemWidth(80.f);
            ImGui::InputInt("Total rows", &_sheetRows);
            ImGui::SetNextItemWidth(80.f);
            ImGui::InputInt("Row to import", &_sheetSelectedRow);
            _sheetColumns = std::max(_sheetColumns, 1);
            _sheetRows = std::max(_sheetRows, 1);
            _sheetSelectedRow = std::clamp(_sheetSelectedRow, 1, _sheetRows);
            if (ImGui::Button("Import selected row"))
            {
                _draft.addFramesFromSpriteSheet(
                    static_cast<std::size_t>(_sheetColumns),
                    static_cast<std::size_t>(_sheetRows),
                    static_cast<std::size_t>(_sheetColumns),
                    static_cast<std::size_t>(_sheetSelectedRow - 1));
            }
        }

        ImGui::Separator();
        const bool nameConflict = [&]
        {
            auto* animator = dynamic_cast<Animation::FrameByFrameAnimator*>(getTargetComponent());
            return animator && animator->hasAnimation(_draft.getAnimationName())
                && (_isCreating || _draft.getAnimationName() != _editedName);
        }();
        const bool missingUvBase = !regionNames.empty() && _draft.getTextureName().isEmpty()
            && std::ranges::any_of(_draft.getFrames(),
                                   [](const auto& frame) { return !frame.textureName; });
        if (!_draft.isValid() || missingUvBase)
        {
            ImGui::TextColored({1.f, .45f, .35f, 1.f},
                               missingUvBase
                                   ? "Select UV base region for frames without their own region."
                                   : "Name, atlas and at least one valid frame are required.");
        }
        else if (nameConflict)
        {
            ImGui::TextColored({1.f, .45f, .35f, 1.f}, "Animation name already exists.");
        }

        ImGui::BeginDisabled(!_draft.isValid() || missingUvBase || nameConflict);
        if (ImGui::Button("Save"))
        {
            auto* animator = dynamic_cast<Animation::FrameByFrameAnimator*>(getTargetComponent());
            if (animator)
            {
                const bool wasCurrentAnimation
                    = !_isCreating && animator->getCurrentAnimationName() == _editedName;
                if (!_isCreating && _editedName != _draft.getAnimationName())
                {
                    animator->removeAnimation(_editedName);
                }
                animator->addAnimation(_draft);
                if (wasCurrentAnimation)
                {
                    animator->startAnimation(_draft.getAnimationName());
                }
                makeParentDirty();
                _showEditor = false;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            _showEditor = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    void ECSEditorFrameByFrameAnimationAdapter::onDraw(float dt)
    {
        if (Gui::CollapsingHeader("Frame-by-frame animations",
                                  ImGuiTreeNodeFlags_DefaultOpen))
        {
            _layout.tick(dt);
            drawAnimationCards();
        }
        drawAnimationEditor();
    }
} // namespace Core
