// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Spectator.h"

#include "Core/Math.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Framework/Input/EditorController.h"
#include "NxWorld/Framework/Input/HumanController.h"

namespace
{
    NX::BaseController::Ptr CreateSpectatorController()
    {
        if (gGameInstance && gGameInstance->isEditorMode())
        {
            NX::EditorController::Ptr controller
                = new NX::EditorController("Spectator controller"_atom);
            controller->getInput().setInputContext(NX::InputContext::EditorViewport);
            return controller;
        }

        NX::HumanController::Ptr controller = new NX::HumanController("Spectator controller"_atom);
        (void)controller->getInput();
        return controller;
    }

    template<class SpectatorT>
    void DeserializeSpectator(RResourceStream<RJsonResourceStream>& stream, SpectatorT& spectator)
    {
        spectator.onPreDeserialize(&spectator);
        // Inspect the restored children before the post hook can create a default input child.
        R<SpectatorT>::Deserialize(stream, spectator, true);
        const auto& data = stream.getData();
        if (!spectator.getInputController()
            && (data.contains("speed") || data.contains("mouseSensitivity")))
        {
            auto controller = CreateSpectatorController();
            auto* input = controller->findFirstChildOf<NX::InputController>();
            if (data.contains("speed"))
            {
                input->speed = data.at("speed").get<float>();
            }
            if (data.contains("mouseSensitivity"))
            {
                input->mouseSensitivity = data.at("mouseSensitivity").get<float>();
            }
            // As with deserialized children, the post hook attaches this child's parent. This
            // also preserves migrated settings when a disabled spectator is saved before init.
            spectator.getChildren().push_back(controller.get());
            spectator.invalidate();
        }
        spectator.onPostDeserialize(&spectator, stream.logs());
    }
} // namespace

namespace NX
{

    ECS_IMPL_NO_SER(BaseSpectator);
    ECS_IMPL_NO_SER(Spectator2D);
    ECS_IMPL_NO_SER(Spectator3D);

    nlohmann::json BaseSpectator::serialize() const
    {
        return R<BaseSpectator>::Serialize(*this).getData();
    }

    void BaseSpectator::deserialize(RResourceStream<RJsonResourceStream>& stream)
    {
        DeserializeSpectator(stream, *this);
    }

    nlohmann::json Spectator2D::serialize() const
    {
        return R<Spectator2D>::Serialize(*this).getData();
    }

    void Spectator2D::deserialize(RResourceStream<RJsonResourceStream>& stream)
    {
        DeserializeSpectator(stream, *this);
    }

    nlohmann::json Spectator3D::serialize() const
    {
        return R<Spectator3D>::Serialize(*this).getData();
    }

    void Spectator3D::deserialize(RResourceStream<RJsonResourceStream>& stream)
    {
        DeserializeSpectator(stream, *this);
    }

    Core::StringAtom BaseSpectator::getCacheHash() const
    {
        return "EditorsRootSpectator"_atom;
    }

    InputController* BaseSpectator::getInputController()
    {
        return findFirstChildOf<InputController>();
    }

    const InputController* BaseSpectator::getInputController() const
    {
        return findFirstChildOf<InputController>();
    }

    void BaseSpectator::onInitialize()
    {
        Actor::onInitialize();
        if (getInputController())
        {
            if (auto* controller = findFirstChildOf<HumanController>())
            {
                controller->possess(this);
            }
            return;
        }

        if (gGameInstance && gGameInstance->isEditorMode())
        {
            auto* controller = addChildComponent<EditorController>("Spectator controller"_atom);
            controller->getInput().setInputContext(InputContext::EditorViewport);
        }
        else
        {
            auto* controller = addChildComponent<HumanController>("Spectator controller"_atom);
            controller->possess(this);
        }
    }

    void Spectator3D::onInitialize()
    {
        BaseSpectator::onInitialize();
        auto* input = getInputController();
        if (input->getBindings().empty())
        {
            const auto bindMovement
                = [input](const Core::StringAtom& name, Platform::Keyboard::Key key)
            { input->bind(name, KeyChord::Exact(key), InputActionTrigger::WhileHeld); };
            bindMovement("Move forward"_atom, Platform::Keyboard::Key::W);
            bindMovement("Move backward"_atom, Platform::Keyboard::Key::S);
            bindMovement("Move right"_atom, Platform::Keyboard::Key::D);
            bindMovement("Move left"_atom, Platform::Keyboard::Key::A);
            bindMovement("Move up"_atom, Platform::Keyboard::Key::R);
            bindMovement("Move down"_atom, Platform::Keyboard::Key::F);
            bindMovement("Fast movement"_atom, Platform::Keyboard::Key::Left_Shift);
            bindMovement("Fast movement"_atom, Platform::Keyboard::Key::Right_Shift);
            bindMovement("Slow movement"_atom, Platform::Keyboard::Key::Left_Control);
            bindMovement("Slow movement"_atom, Platform::Keyboard::Key::Right_Control);
            // tag::input-pointer-bindings[]
            auto bindings = input->getBindings();
            bindings.push_back({ .action = "Scroll camera"_atom, .source = InputEventType::Wheel });
            bindings.push_back({ .action = "Rotate camera"_atom,
                                 .chord = KeyChord::Exact(Platform::Mouse::Key::Right),
                                 .source = InputEventType::PointerMove });
            input->setBindings(bindings);
            // end::input-pointer-bindings[]
        }
        input->setActionCallback(
            "Scroll camera"_atom,
            [this, input](const InputActionEvent& event)
            {
                const bool shifted = (event.modifiers & InputModifier_Shift) != InputModifier_None;
                const auto mlt = input->speed / (shifted ? leftShiftMlt : 1.f);
                moveForward(-event.value.y * mlt * gGameInstance->world.getTimeDelta());
            });
        input->setActionCallback("Rotate camera"_atom, [this, input](const InputActionEvent& event)
                                 { yawAndPitch(event.value * input->mouseSensitivity); });
    }

    void Spectator3D::onTick(float delta)
    {
        BaseSpectator::onTick(delta);
        const auto* input = getInputController();
        if (!input)
        {
            return;
        }
        float multiplier = baseMlt;
        if (input->isActive("Fast movement"_atom))
        {
            multiplier = leftShiftMlt;
        }
        if (input->isActive("Slow movement"_atom))
        {
            multiplier = leftCtrlMlt;
        }
        const float step = input->speed * delta * multiplier;
        const auto active = [input](const Core::StringAtom& action)
        { return static_cast<float>(input->isActive(action)); };
        moveForward(step * (active("Move backward"_atom) - active("Move forward"_atom)));
        moveRight(step * (active("Move right"_atom) - active("Move left"_atom)));
        moveUp(step * (active("Move up"_atom) - active("Move down"_atom)));
    }

    void Spectator2D::onInitialize()
    {
        BaseSpectator::onInitialize();

        auto* input = getInputController();
        if (input->getBindings().empty())
        {
            input->setBindings(
                { { .action = "Pan camera"_atom,
                    .chord = KeyChord::Exact(Platform::Mouse::Key::Middle),
                    .source = InputEventType::PointerMove },
                  { .action = "Scroll camera"_atom, .source = InputEventType::Wheel } });
        }
        input->setActionCallback("Pan camera"_atom,
                                 [this](const InputActionEvent& event)
                                 {
                                     const bool shifted = (event.modifiers & InputModifier_Shift)
                                                          != InputModifier_None;
                                     const auto* camera = findFirstChildOf<OrthographicCamera>();
                                     const float zoom = camera ? camera->getZoom() : 1.f;
                                     const float mlt = (shifted ? 5.f : 1.f) / zoom;

                                     moveRight(-event.value.x * mlt);
                                     moveUp(event.value.y * mlt);
                                 });

        input->setActionCallback(
            "Scroll camera"_atom,
            [this, input](const InputActionEvent& event)
            {
                auto offset = event.value;
                float mlt = baseMlt;
                if ((event.modifiers & InputModifier_Shift) != InputModifier_None)
                {
                    mlt = leftShiftMlt;
                }
                offset *= mlt;

                if ((event.modifiers & InputModifier_Control) != InputModifier_None)
                {
                    if (auto* camera = findFirstChildOf<OrthographicCamera>())
                    {
                        const auto zoomStep = (maxZoom - minZoom) / 100.f;
                        const auto scrollStep = std::clamp(offset.y, -1.f, 1.f);
                        auto z = camera->getZoom();

                        auto finalStep = zoomStep * scrollStep * std::max(1.f, sqrtf(z));
                        const auto expectedZoom = z + finalStep;

                        if (expectedZoom < minZoom)
                        {
                            camera->setZoom(minZoom);
                            return;
                        }
                        if (expectedZoom > maxZoom)
                        {
                            camera->setZoom(maxZoom);
                            return;
                        }

                        camera->adjustZoom(finalStep);
                        return;
                    }
                }

                const float adjustedSpeed = input->speed * 10.f;
                const float panSpeed = adjustedSpeed * gGameInstance->world.getTimeDelta();
                moveRight(-offset.x * panSpeed);
                moveUp(offset.y * panSpeed);
            });
    }
} // namespace NX
