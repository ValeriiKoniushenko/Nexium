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

#include "ECSEditorAnimationAudioBindingAdapter.h"

#include "Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "Audio/AudioSource.h"
#include "Editor/GuiComponents/Misc.h"
#include "ImGui/imgui.h"
#include "Misc/IconsFontAwesome.h"

#include <algorithm>
#include <array>
#include <optional>

namespace
{
    using namespace Core;

    void SortAndUnique(std::vector<StringAtom>& values)
    {
        std::ranges::sort(values, [](const StringAtom& lhs, const StringAtom& rhs)
                          { return lhs.toStdString() < rhs.toStdString(); });
        values.erase(std::unique(values.begin(), values.end()), values.end());
    }

    std::vector<StringAtom> GetAnimationNames(const Animation::FrameByFrameAnimator* animator)
    {
        std::vector<StringAtom> names;
        if (!animator)
        {
            return names;
        }

        names.reserve(animator->getAnimations().size());
        for (const auto& [name, animation] : animator->getAnimations())
        {
            if (animation)
            {
                names.push_back(name);
            }
        }
        SortAndUnique(names);
        return names;
    }

    std::vector<StringAtom> GetEventNames(const Animation::FrameByFrameAnimator* animator,
                                          const StringAtom& animationName)
    {
        std::vector<StringAtom> names;
        const auto* animation = animator ? animator->getAnimation(animationName) : nullptr;
        if (!animation)
        {
            return names;
        }

        for (const auto& frame : animation->getFrames())
        {
            names.insert(names.end(), frame.events.begin(), frame.events.end());
        }
        SortAndUnique(names);
        return names;
    }

    std::vector<StringAtom> GetAudioSourceNames(const BaseComponent* owner)
    {
        std::vector<StringAtom> names;
        if (!owner)
        {
            return names;
        }

        owner->forEach(
            [&names](const BaseComponent* component)
            {
                if (const auto* source = dynamic_cast<const Audio::AudioSource*>(component);
                    source && !source->getComponentName().isEmpty())
                {
                    names.push_back(source->getComponentName());
                }
            });
        SortAndUnique(names);
        return names;
    }

    [[nodiscard]] bool Contains(const std::vector<StringAtom>& values, const StringAtom& value)
    {
        return std::ranges::find(values, value) != values.end();
    }

    void DrawStringSelector(const char* label, StringAtom& selected,
                            const std::vector<StringAtom>& values)
    {
        const char* preview = selected.isEmpty() ? "<Select>" : selected.c_str();
        if (!ImGui::BeginCombo(label, preview))
        {
            return;
        }

        for (const auto& value : values)
        {
            const bool isSelected = value == selected;
            if (ImGui::Selectable(value.c_str(), isSelected))
            {
                selected = value;
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        if (values.empty())
        {
            ImGui::TextDisabled("No values available");
        }
        ImGui::EndCombo();
    }

    [[nodiscard]] const char* GetCommandName(const Audio::AudioCommand command)
    {
        switch (command)
        {
            case Audio::AudioCommand::Play:
                return "Play";
            case Audio::AudioCommand::Pause:
                return "Pause";
            case Audio::AudioCommand::Resume:
                return "Resume";
            case Audio::AudioCommand::Stop:
                return "Stop";
        }
        return "Invalid";
    }

    void DrawCommandSelector(Audio::AudioCommand& selected)
    {
        constexpr std::array commands{ Audio::AudioCommand::Play, Audio::AudioCommand::Pause,
                                       Audio::AudioCommand::Resume, Audio::AudioCommand::Stop };
        if (!ImGui::BeginCombo("Command", GetCommandName(selected)))
        {
            return;
        }

        for (const auto command : commands)
        {
            const bool isSelected = command == selected;
            if (ImGui::Selectable(GetCommandName(command), isSelected))
            {
                selected = command;
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
} // namespace

namespace Core
{
    ECS_IMPL(ECSEditorAnimationAudioBindingAdapter);

    bool ECSEditorAnimationAudioBindingAdapter::canWorkWith(BaseComponent* component) const
    {
        return dynamic_cast<Audio::AnimationAudioBinding*>(component) != nullptr;
    }

    StringAtom ECSEditorAnimationAudioBindingAdapter::getProcessedAssetType() const
    {
        return Audio::AnimationAudioBinding::componentType;
    }

    void ECSEditorAnimationAudioBindingAdapter::onEditorTargetChanged()
    {
        _draftCues.clear();
        _isCueEditorOpen = false;
    }

    void ECSEditorAnimationAudioBindingAdapter::openCueEditor(
        const Audio::AnimationAudioBinding& binding)
    {
        _draftCues = binding.getCues();
        _isCueEditorOpen = true;
    }

    void ECSEditorAnimationAudioBindingAdapter::onDraw(float)
    {
        auto* binding = dynamic_cast<Audio::AnimationAudioBinding*>(getTargetComponent());
        if (!binding)
        {
            return;
        }

        if (!Gui::CollapsingHeader("Animation audio binding", ImGuiTreeNodeFlags_DefaultOpen))
        {
            return;
        }

        ImGui::TextDisabled("%zu cue(s)", binding->getCues().size());
        if (ImGui::Button(ICON_FA_PENCIL " Edit cues"))
        {
            openCueEditor(*binding);
        }
        drawCueEditor(*binding);
    }

    void ECSEditorAnimationAudioBindingAdapter::drawCueEditor(Audio::AnimationAudioBinding& binding)
    {
        if (!_isCueEditorOpen)
        {
            return;
        }

        const auto* owner = binding.getOwner();
        const auto* animator
            = owner ? owner->findFirstChildOf<Animation::FrameByFrameAnimator>() : nullptr;
        const auto animationNames = GetAnimationNames(animator);
        const auto sourceNames = GetAudioSourceNames(owner);

        ImGui::OpenPopup("Animation audio cues");
        if (!ImGui::BeginPopupModal("Animation audio cues", &_isCueEditorOpen,
                                    ImGuiWindowFlags_AlwaysAutoResize))
        {
            return;
        }

        if (!animator)
        {
            ImGui::TextColored({ 1.f, .45f, .35f, 1.f },
                               "No FrameByFrameAnimator exists under this object.");
        }
        if (sourceNames.empty())
        {
            ImGui::TextColored({ 1.f, .45f, .35f, 1.f },
                               "No named AudioSource exists under this object.");
        }
        ImGui::TextDisabled("AudioSource component names must be unique under this object.");

        std::optional<std::size_t> cueToDelete;
        for (std::size_t index = 0; index < _draftCues.size(); ++index)
        {
            auto& cue = _draftCues[index];
            ImGui::PushID(static_cast<int>(index));
            ImGui::Separator();
            ImGui::Text("Cue %zu", index + 1);
            ImGui::SameLine();
            if (ImGui::SmallButton(ICON_FA_TRASH))
            {
                cueToDelete = index;
            }

            DrawStringSelector("Animation", cue.animationName, animationNames);
            const auto eventNames = GetEventNames(animator, cue.animationName);
            DrawStringSelector("Event", cue.eventName, eventNames);
            DrawStringSelector("Audio source", cue.audioSourceName, sourceNames);
            DrawCommandSelector(cue.command);

            const bool hasAnimation = Contains(animationNames, cue.animationName);
            const bool hasEvent = hasAnimation && Contains(eventNames, cue.eventName);
            const bool hasSource = Contains(sourceNames, cue.audioSourceName);
            if (!hasAnimation)
            {
                ImGui::TextColored({ 1.f, .45f, .35f, 1.f }, "Animation is missing or renamed.");
            }
            else if (!hasEvent)
            {
                ImGui::TextColored({ 1.f, .45f, .35f, 1.f }, "Event is missing or renamed.");
            }
            if (!hasSource)
            {
                ImGui::TextColored({ 1.f, .45f, .35f, 1.f },
                                   "Audio source is missing or renamed.");
            }
            if (!cue.isValid())
            {
                ImGui::TextColored({ 1.f, .45f, .35f, 1.f }, "Cue needs an animation, event, and source.");
            }
            ImGui::PopID();
        }
        if (cueToDelete)
        {
            _draftCues.erase(_draftCues.begin() + *cueToDelete);
        }

        const auto defaultEvents = animationNames.empty()
                                       ? std::vector<StringAtom>{}
                                       : GetEventNames(animator, animationNames.front());
        const bool canAddCue
            = !animationNames.empty() && !defaultEvents.empty() && !sourceNames.empty();
        ImGui::Separator();
        ImGui::BeginDisabled(!canAddCue);
        if (ImGui::Button(ICON_FA_PLUS " Add cue"))
        {
            _draftCues.emplace_back(Audio::AnimationAudioCue{ .animationName = animationNames.front(),
                                                                .eventName = defaultEvents.front(),
                                                                .audioSourceName = sourceNames.front(),
                                                                .command = Audio::AudioCommand::Play });
        }
        ImGui::EndDisabled();
        if (!canAddCue)
        {
            ImGui::TextDisabled("Add an animation marker and a named AudioSource first.");
        }

        const bool hasInvalidCue = std::ranges::any_of(
            _draftCues, [](const Audio::AnimationAudioCue& cue) { return !cue.isValid(); });
        ImGui::SameLine();
        ImGui::BeginDisabled(hasInvalidCue);
        if (ImGui::Button("Save cues"))
        {
            binding.clearCues();
            for (const auto& cue : _draftCues)
            {
                binding.addCue(cue);
            }
            makeParentDirty();
            _isCueEditorOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            _draftCues.clear();
            _isCueEditorOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
} // namespace Core
