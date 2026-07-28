#include "FrameByFrameAnimationEditor.h"

#include "Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Misc/IconsFontAwesome.h"

#include <algorithm>
#include <cstring>

#include "ImGui/imgui.h"

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
    void FrameByFrameAnimationEditor::create(Animation::FrameByFrameAnimator* animator)
    {
        _animator = animator;
        _draft = Animation::FrameByFrameAnimation{};
        _draft.setLoop(true);
        _editedName.clear();
        _isCreating = true;
        _isOpen = animator != nullptr;
        copyDraftToBuffers();
        std::strncpy(_name.data(), "New animation", _name.size() - 1);
    }

    void FrameByFrameAnimationEditor::edit(Animation::FrameByFrameAnimator* animator,
                                           const StringAtom& name)
    {
        _animator = animator;
        if (!_animator)
        {
            return;
        }
        if (const auto* animation = _animator->getAnimation(name))
        {
            _draft = *animation;
            _editedName = name;
            _isCreating = false;
            _isOpen = true;
            copyDraftToBuffers();
        }
    }

    void FrameByFrameAnimationEditor::copyDraftToBuffers()
    {
        CopyToBuffer(_name, _draft.getAnimationName());
        CopyToBuffer(_atlas, _draft.getAtlasName());
        CopyToBuffer(_texture, _draft.getTextureName());
        _frameTexture.fill('\0');
        _fps = _draft.getFPS();
        _loop = _draft.isLooping();
        _preview = {};
    }

    void FrameByFrameAnimationEditor::applyBuffersToDraft()
    {
        _draft.setAnimationName(StringAtom{_name.data()});
        _draft.setAtlasName(StringAtom{_atlas.data()});
        _draft.setTextureName(StringAtom{_texture.data()});
        _draft.setFPS(std::max(_fps, 0.01f));
        _draft.setLoop(_loop);
    }

    void FrameByFrameAnimationEditor::drawPreview(
        const Animation::FrameByFrameAnimation& animation, float dt, float size,
        PreviewState& state)
    {
        const auto& frames = animation.getFrames();
        if (frames.empty())
        {
            ImGui::TextDisabled("Preview: no frames");
            return;
        }

        state.frame %= frames.size();
        state.timer += dt;
        const float frameTime = 1.f / std::max(animation.getFPS(), 0.01f);
        while (state.timer >= frameTime)
        {
            state.timer -= frameTime;
            state.frame = (state.frame + 1) % frames.size();
        }

        const auto atlasNames = GetAssetsManager().getAtlasesAsVector();
        if (std::ranges::find(atlasNames, animation.getAtlasName()) == atlasNames.end())
        {
            ImGui::TextDisabled("Preview unavailable (atlas '%s' is not loaded)",
                                animation.getAtlasName().c_str());
            return;
        }

        const auto& atlas = GetAssetsManager().getAtlas(animation.getAtlasName());
        const auto& frame = frames[state.frame];
        glm::vec2 uv0{frame.uvOffset};
        glm::vec2 uv1 = uv0 + glm::vec2{frame.uvSize};
        const StringAtom regionName = frame.textureName.value_or(animation.getTextureName());
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
        ImGui::Text("%zu / %zu", state.frame + 1, frames.size());
    }

    void FrameByFrameAnimationEditor::drawFrames()
    {
        if (!ImGui::CollapsingHeader("Frames", ImGuiTreeNodeFlags_DefaultOpen))
        {
            return;
        }

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
            const auto fallbackName = "Frame "_atom + StringAtom::MakeFrom(i + 1);
            ImGui::TextUnformatted(frame.name.isEmpty() ? fallbackName.c_str()
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

    void FrameByFrameAnimationEditor::drawNamedFrames(
        const std::vector<StringAtom>& regionNames)
    {
        if (!ImGui::CollapsingHeader("Named atlas frames", ImGuiTreeNodeFlags_DefaultOpen))
        {
            return;
        }
        if (regionNames.empty())
        {
            ImGui::TextDisabled("The selected atlas has no named regions.");
            return;
        }

        const char* preview = _frameTexture[0] ? _frameTexture.data() : "Select region";
        if (ImGui::BeginCombo("Atlas region", preview))
        {
            for (const auto& regionName : regionNames)
            {
                const bool selected = regionName == StringAtom{_frameTexture.data()};
                if (ImGui::Selectable(regionName.c_str(), selected))
                {
                    CopyToBuffer(_frameTexture, regionName);
                }
                if (selected)
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

    void FrameByFrameAnimationEditor::drawSpriteSheet()
    {
        if (!ImGui::CollapsingHeader("Sprite sheet / UV frames",
                                     ImGuiTreeNodeFlags_DefaultOpen))
        {
            return;
        }
        ImGui::TextDisabled("Import one complete row from a uniformly spaced sprite sheet.");
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
                static_cast<std::size_t>(_sheetRows - _sheetSelectedRow));
        }
    }

    bool FrameByFrameAnimationEditor::hasNameConflict() const
    {
        return _animator && _animator->hasAnimation(_draft.getAnimationName())
            && (_isCreating || _draft.getAnimationName() != _editedName);
    }

    void FrameByFrameAnimationEditor::draw(float dt, const SaveCallback& onSave)
    {
        if (!_isOpen || !_animator)
        {
            return;
        }

        ImGui::OpenPopup("Frame-by-frame animation editor");
        if (!ImGui::BeginPopupModal("Frame-by-frame animation editor", &_isOpen,
                                    ImGuiWindowFlags_AlwaysAutoResize))
        {
            return;
        }

        ImGui::InputText("Name", _name.data(), _name.size());
        const auto atlasNames = GetAssetsManager().getAtlasesAsVector();
        const char* atlasPreview = _atlas[0] ? _atlas.data() : "Select atlas";
        if (ImGui::BeginCombo("Atlas", atlasPreview))
        {
            for (const auto& atlasName : atlasNames)
            {
                const bool selected = atlasName == StringAtom{_atlas.data()};
                if (ImGui::Selectable(atlasName.c_str(), selected))
                {
                    CopyToBuffer(_atlas, atlasName);
                    _texture.fill('\0');
                    _frameTexture.fill('\0');
                }
                if (selected)
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
        if (_atlas[0]
            && std::ranges::find(atlasNames, StringAtom{_atlas.data()}) != atlasNames.end())
        {
            regionNames
                = GetAssetsManager().getAtlas(StringAtom{_atlas.data()}).getRectsAsVector();
        }
        if (!regionNames.empty())
        {
            const char* basePreview = _texture[0] ? _texture.data() : "Select base region";
            if (ImGui::BeginCombo("UV base region", basePreview))
            {
                for (const auto& regionName : regionNames)
                {
                    const bool selected = regionName == StringAtom{_texture.data()};
                    if (ImGui::Selectable(regionName.c_str(), selected))
                    {
                        CopyToBuffer(_texture, regionName);
                    }
                    if (selected)
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
        drawPreview(_draft, dt, 180.f, _preview);
        drawFrames();
        drawNamedFrames(regionNames);
        drawSpriteSheet();

        const bool missingUvBase = !regionNames.empty() && _draft.getTextureName().isEmpty()
            && std::ranges::any_of(_draft.getFrames(),
                                   [](const auto& frame) { return !frame.textureName; });
        const bool nameConflict = hasNameConflict();
        ImGui::Separator();
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
            const bool wasCurrent
                = !_isCreating && _animator->getCurrentAnimationName() == _editedName;
            if (!_isCreating && _editedName != _draft.getAnimationName())
            {
                _animator->removeAnimation(_editedName);
            }
            _animator->addAnimation(_draft);
            if (wasCurrent)
            {
                _animator->startAnimation(_draft.getAnimationName());
            }
            if (onSave)
            {
                onSave();
            }
            _isOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            _isOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
} // namespace Core
