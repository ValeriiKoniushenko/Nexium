/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 */

#include "ECSEditorFrameByFrameAnimationAdapter.h"

#include "Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/Misc.h"
#include "Misc/IconsFontAwesome.h"

#include <algorithm>

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
        _layout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });
        _addAnimButton = _layout.addChildComponent<Gui::Button>();
        _addAnimButton->setText(ICON_FA_PLUS " Add animation");
        _addAnimButton->setFlex(Gui::Flex::FlexWidth);
        _subscriptionPool << _addAnimButton->onClick->subscribeAndGetID(
            [this]
            {
                _animationEditor.create(
                    dynamic_cast<Animation::FrameByFrameAnimator*>(getTargetComponent()));
            });
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
        std::ranges::sort(names, [](const auto& lhs, const auto& rhs)
                          { return lhs.toStdString() < rhs.toStdString(); });

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
            FrameByFrameAnimationEditor::drawPreview(*animation, ImGui::GetIO().DeltaTime, 112.f,
                                                     _previewStates["card:"_atom + name]);
            if (ImGui::Button(ICON_FA_PENCIL " Edit"))
            {
                _animationEditor.edit(animator, name);
            }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_TRASH " Delete"))
            {
                animator->removeAnimation(name);
                _previewStates.erase("card:"_atom + name);
                makeParentDirty();
            }
            ImGui::EndGroup();
            ImGui::Separator();
            ImGui::PopID();
        }
    }

    void ECSEditorFrameByFrameAnimationAdapter::onDraw(float dt)
    {
        if (Gui::CollapsingHeader("Frame-by-frame animations", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _layout.tick(dt);
            drawAnimationCards();
        }
        _animationEditor.draw(dt,
                              [this]
                              {
                                  if (auto* editor = getParentAs<NxECSBasedEditorEWC>())
                                  {
                                      editor->save();
                                  }
                              });
    }
} // namespace Core
