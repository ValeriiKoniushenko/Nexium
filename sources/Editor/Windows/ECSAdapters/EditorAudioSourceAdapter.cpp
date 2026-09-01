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

#include "EditorAudioSourceAdapter.h"

#include "AssetsManager/AssetsManager.h"
#include "Editor/GuiComponents/Misc.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "ImGui/imgui.h"
#include "Misc/IconsFontAwesome.h"

namespace Core
{
    ECS_IMPL(ECSEditorAudioSourceAdapter);

    bool ECSEditorAudioSourceAdapter::canWorkWith(BaseComponent* component) const
    {
        return dynamic_cast<Audio::AudioSource*>(component) != nullptr;
    }

    StringAtom ECSEditorAudioSourceAdapter::getProcessedAssetType() const
    {
        return Audio::AudioSource::componentType;
    }

    void ECSEditorAudioSourceAdapter::onEditorTargetChanged()
    {
        stopPreview();
    }

    void ECSEditorAudioSourceAdapter::onInitialize()
    {
        ECSEditorMimeAdapter::onInitialize();
    }

    void ECSEditorAudioSourceAdapter::onApplyAssetData(const nlohmann::json&)
    {
        auto* source = dynamic_cast<Audio::AudioSource*>(getTargetComponent());
        if (!source)
        {
            stopPreview();
        }
    }

    void ECSEditorAudioSourceAdapter::onDraw(float)
    {
        auto* source = dynamic_cast<Audio::AudioSource*>(getTargetComponent());
        if (!source)
        {
            return;
        }

        if (!Gui::CollapsingHeader("Audio source", ImGuiTreeNodeFlags_DefaultOpen))
        {
            return;
        }

        const auto clips = GetAssetsManager().getAudioClipsAsVector();
        const auto& currentClip = source->getClipPath();
        const char* preview = currentClip.isEmpty() ? "<None>" : currentClip.c_str();
        if (ImGui::BeginCombo("Clip", preview))
        {
            const bool noClipSelected = currentClip.isEmpty();
            if (ImGui::Selectable("<None>", noClipSelected))
            {
                stopPreview();
                source->setClip({});
                makeParentDirty();
            }
            if (noClipSelected)
            {
                ImGui::SetItemDefaultFocus();
            }

            for (const auto& clip : clips)
            {
                const bool isSelected = clip == currentClip;
                if (ImGui::Selectable(clip.c_str(), isSelected))
                {
                    stopPreview();
                    source->setClip(clip);
                    makeParentDirty();
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        float volume = source->getVolume();
        if (ImGui::SliderFloat("Volume", &volume, Audio::AudioSource::minVolume,
                               Audio::AudioSource::maxVolume))
        {
            source->setVolume(volume);
            makeParentDirty();
        }

        bool loop = source->isLooping();
        if (ImGui::Checkbox("Loop", &loop))
        {
            source->setLooping(loop);
            makeParentDirty();
        }

        int polyphony = static_cast<int>(source->getMaxPolyphony());
        ImGui::BeginDisabled(loop);
        if (ImGui::SliderInt("Max polyphony", &polyphony,
                             static_cast<int>(Audio::AudioSource::minPolyphony),
                             static_cast<int>(Audio::AudioSource::maxPolyphony)))
        {
            source->setMaxPolyphony(static_cast<std::uint32_t>(polyphony));
            makeParentDirty();
        }
        ImGui::EndDisabled();
        if (loop)
        {
            ImGui::TextDisabled("Looping sources always use one voice.");
        }

        ImGui::Spacing();
        ImGui::TextUnformatted("Preview");
        if (ImGui::Button(ICON_FA_PLAY " Play"))
        {
            if (source->play().isValid())
            {
                _previewSource = source;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_PAUSE " Pause"))
        {
            source->pause();
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_PLAY " Resume"))
        {
            source->resume();
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_STOP " Stop"))
        {
            source->stop();
            if (_previewSource == source)
            {
                _previewSource = nullptr;
            }
        }
    }

    void ECSEditorAudioSourceAdapter::stopPreview()
    {
        if (_previewSource)
        {
            _previewSource->stop();
            _previewSource = nullptr;
        }
    }
} // namespace Core
