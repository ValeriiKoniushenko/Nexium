// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "EditorPropertyCustomizations.h"

#include "Editor/EditorIntegration.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/EnumInputs.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/PropertyCustomization.h"
#include "Editor/GuiComponents/PropertyGrid.h"
#include "Editor/IconsFontAwesome.h"
#include "Editor/ToastNotifications.h"
#include "Editor/Windows/Scenes/SceneTreeWindow.h"
#include "NxFundamental/Transformable.h"
#include "NxFundamental/Utils/Stringifier.h"
#include "NxSubsystems/AssetsManager/AssetsManager.h"
#include "NxSubsystems/Graphics/GraphicsComponents.h"
#include "NxWorld/Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Framework/GameInstanceAccess.h"
#include "NxWorld/Framework/World.h"
#include "NxWorld/Scene/Scene.h"
#include "NxWorld/Scene/SceneManager.h"
#include "NxWorld/Scene/SceneObjects/Rectangle/Rectangle.h"
#include "NxWorld/Scene/SceneObjects/Rectangle/RectangleAnimated.h"

#include <algorithm>
#include <format>
#include <string>
#include <tuple>
#include <vector>

namespace NX
{
    namespace
    {
        using Gui::ChoiceRequest;
        using Gui::CustomRowContext;
        using Gui::PropertyEditorContext;
        using SceneObj::Rectangle;
        using SceneObj::RectangleAnimated;

        constexpr std::string_view ComponentData = "Component data";
        constexpr std::string_view Rendering = "Rendering";

        /// The callback lives as long as the widget that owns the delegate.
        template<class DelegatePtr, class F>
        void Listen(DelegatePtr& delegate, F&& callback)
        {
            std::ignore = delegate->subscribeAndGetID(std::forward<F>(callback));
        }

        [[nodiscard]] Core::StringAtom NameAndType(const BaseComponent& component)
        {
            return Core::StringAtom(std::format("{} ({})",
                                                component.getComponentName().toStdStringView(),
                                                component.getComponentType().toStdStringView()));
        }

        // ===================== Components =====================

        void CustomizeComponents(Gui::PropertyCustomizations& customizations)
        {
            customizations.For<AbstractComponent>().section("General");
            customizations.For<BaseComponent>()
                .section("General")
                .computed<Core::StringAtom>("parent", "Parent",
                                            [](BaseComponent& component)
                                            {
                                                return component.hasParent()
                                                           ? NameAndType(*component.getParent())
                                                           : Core::StringAtom(
                                                                 Stringify::gcDefaultNoneString);
                                            },
                                            { .category = ComponentData })
                .computed<std::size_t>(
                    "childrenCount", "Children count", [](BaseComponent& component)
                    { return component.getChildrenCount(); }, { .category = ComponentData })
                .computed<bool>("isInitialized", "Is initialized",
                                [](BaseComponent& component) { return component.isInitialized(); },
                                { .category = ComponentData })
                .order({ "parent", "childrenCount", "_children", "isInitialized" });

            customizations.For<Transformable>().section("Transform");
            customizations.For<RawBackend::IDrawable>().section(Rendering);
        }

        // ===================== Cameras =====================

        void CustomizeCameras(Gui::PropertyCustomizations& customizations)
        {
            customizations.For<BaseCamera>().section("Camera");
            customizations.For<OrthographicCamera>().section("Camera");
            customizations.For<PerspectiveCamera>()
                .section("Camera")
                .computed<Core::FSize2>("outputSize", "Output size",
                                        [](PerspectiveCamera& camera)
                                        {
                                            // The render size needs a running game.
                                            return gGameInstance ? camera.getOutputFrameSize()
                                                                 : Core::FSize2(0.f);
                                        })
                .order({ "_fov", "_aspect", "outputSize" });
        }

        // ===================== Rectangles =====================

        [[nodiscard]] bool HasAtlas(const AssetsManager& assets, const Core::StringAtom& atlas)
        {
            return std::ranges::contains(assets.getAtlasesAsVector(), atlas);
        }

        /// A new atlas starts on its first frame (the old frame belongs to the old atlas).
        void SelectFirstFrame(Rectangle& rectangle)
        {
            auto* assets = GetAssetsManager();
            if (!assets || !HasAtlas(*assets, rectangle.getAtlasName()))
            {
                return;
            }
            const auto frames = assets->getAtlas(rectangle.getAtlasName()).getRectsAsVector();
            if (!frames.empty())
            {
                rectangle.setTexture(frames.front());
            }
        }

        [[nodiscard]] const Animation::FrameByFrameAnimator* AnimatorOf(
            const RectangleAnimated& rectangle)
        {
            return rectangle.findFirstChildOf<Animation::FrameByFrameAnimator>();
        }

        [[nodiscard]] Animation::FrameByFrameAnimator* AnimatorOf(RectangleAnimated& rectangle)
        {
            return rectangle.findFirstChildOf<Animation::FrameByFrameAnimator>();
        }

        [[nodiscard]] std::vector<Core::StringAtom> AnimationNames(
            const Animation::FrameByFrameAnimator& animator)
        {
            std::vector<Core::StringAtom> names;
            names.reserve(animator.getAnimations().size());
            for (const auto& [name, animation] : animator.getAnimations())
            {
                names.push_back(name);
            }
            std::ranges::sort(names, {},
                              [](const Core::StringAtom& name) { return name.toStdStringView(); });
            return names;
        }

        void CustomizeRectangles(Gui::PropertyCustomizations& customizations,
                                 Gui::PropertyChoices& choices)
        {
            customizations.For<Rectangle>()
                .section("Texture")
                .order({ "_atlasName", "_textureName" })
                .property<bool>(
                    "blending", "Blending",
                    [](Rectangle& rectangle) { return rectangle.isBlendingEnabled(); },
                    [](Rectangle& rectangle, const bool& value)
                    { rectangle.setBlendingEnabled(value); }, { .category = Rendering })
                .afterEdit("_atlasName", &SelectFirstFrame);

            customizations.For<RectangleAnimated>()
                .section("Animation")
                .property<bool>(
                    "animationEnabled", "Animated",
                    [](RectangleAnimated& rectangle) { return rectangle.isAnimationEnabled(); },
                    [](RectangleAnimated& rectangle, const bool& value)
                    { rectangle.setAnimationEnabled(value); })
                .property<Core::StringAtom>(
                    "animation", "Animation",
                    [](RectangleAnimated& rectangle)
                    {
                        const auto* animator = AnimatorOf(rectangle);
                        return animator ? animator->getActiveAnimationName() : Core::StringAtom{};
                    },
                    [](RectangleAnimated& rectangle, const Core::StringAtom& name)
                    {
                        if (auto* animator = AnimatorOf(rectangle))
                        {
                            if (const auto* animation = animator->getAnimation(name))
                            {
                                rectangle.setAnimationOverride(name, animation->getFPS());
                            }
                        }
                    },
                    { .meta = { { "Editor", "AssetRef" }, { "AssetType", "RectangleAnimation" } } })
                .property<float>(
                    "animationFps", "Animation FPS",
                    [](RectangleAnimated& rectangle)
                    {
                        const auto* animator = AnimatorOf(rectangle);
                        const auto* animation = animator ? animator->getActiveAnimation() : nullptr;
                        return animation ? animation->getFPS() : 0.f;
                    },
                    [](RectangleAnimated& rectangle, const float& fps)
                    {
                        auto* animator = AnimatorOf(rectangle);
                        auto* animation = animator ? animator->getActiveAnimation() : nullptr;
                        if (animation)
                        {
                            animation->setFPS(fps);
                            rectangle.setAnimationOverride(animator->getActiveAnimationName(), fps);
                        }
                    },
                    { .min = 0.01, .max = 1000.0, .step = 0.25 })
                .enabledIf("animation",
                           [](const RectangleAnimated& rectangle)
                           {
                               const auto* animator = AnimatorOf(rectangle);
                               return rectangle.isAnimationEnabled() && animator
                                      && !animator->getAnimations().empty();
                           })
                .enabledIf("animationFps",
                           [](const RectangleAnimated& rectangle)
                           {
                               const auto* animator = AnimatorOf(rectangle);
                               return rectangle.isAnimationEnabled() && animator
                                      && animator->getActiveAnimation();
                           });

            // The animation editor section of the asset editor manages the animations.
            customizations.For<Animation::FrameByFrameAnimator>()
                .hide("_animations")
                .readOnly("_currentAnimationName");

            choices.add(
                "RectangleAnimation"_atom,
                [](const ChoiceRequest& request) -> std::vector<Core::StringAtom>
                {
                    const auto* rectangle = static_cast<const RectangleAnimated*>(request.owner);
                    const auto* animator = rectangle ? AnimatorOf(*rectangle) : nullptr;
                    return animator ? AnimationNames(*animator) : std::vector<Core::StringAtom>{};
                });
        }

        // ===================== Graphics data =====================

        void ModifierParamEditor(PropertyEditorContext& context)
        {
            using Param = BaseGraphicsData::ModifierParam;
            auto* modifier = context.row.addChildComponent<Gui::EnumCombo>();
            modifier->setFlex(Gui::Flex::FlexWidth);
            modifier->setOptions(Gui::EnumOptionsOf<BaseGraphicsData::Modifier>());
            auto* value = context.row.addChildComponent<Gui::EnumCombo>();
            value->setFlex(Gui::Flex::FlexWidth);
            value->setOptions(Gui::EnumOptionsOf<BaseGraphicsData::ModifiedValue>());

            Listen(modifier->onSelect,
                   [binding = context.binding](std::uint64_t bits)
                   {
                       if (const Param* current = binding.get<Param>())
                       {
                           Param next = *current;
                           next.modifier = static_cast<BaseGraphicsData::Modifier>(bits);
                           binding.set(next);
                       }
                   });
            Listen(value->onSelect,
                   [binding = context.binding](std::uint64_t bits)
                   {
                       if (const Param* current = binding.get<Param>())
                       {
                           Param next = *current;
                           next.value = static_cast<BaseGraphicsData::ModifiedValue>(bits);
                           binding.set(next);
                       }
                   });
            context.row.setPull(
                [modifier, value, binding = context.binding]
                {
                    if (const Param* current = binding.get<Param>())
                    {
                        modifier->setValue(Gui::EnumBits(current->modifier));
                        value->setValue(Gui::EnumBits(current->value));
                    }
                });
        }

        void CustomizeGraphics(Gui::PropertyCustomizations& customizations,
                               Gui::PropertyEditors& editors)
        {
            editors.add<BaseGraphicsData::ModifierParam>("Modifier"_atom, &ModifierParamEditor);

            customizations.For<BaseGraphicsData>()
                .section("Graphics")
                .label("_drawModifiers", "Graphic modifier")
                .computed<Core::StringAtom>("shader", "Shader name",
                                            [](BaseGraphicsData& graphics)
                                            {
                                                auto* shader = graphics.getShader();
                                                return shader ? shader->getName()
                                                              : Core::StringAtom(
                                                                    Stringify::gcDefaultNoneString);
                                            })
                .computed<GLuint>("vbo", "VBO ID",
                                  [](BaseGraphicsData& graphics) { return graphics.getVboId(); })
                .computed<GLuint>("vao", "VAO ID",
                                  [](BaseGraphicsData& graphics) { return graphics.getVaoId(); })
                .computed<GLuint>("ebo", "EBO ID",
                                  [](BaseGraphicsData& graphics) { return graphics.getEboId(); })
                .computed<std::uint32_t>("triangles", "Triangles", [](BaseGraphicsData& graphics)
                                         { return graphics.getTriangleCount(); })
                .order({ "triangles", "shader", "vbo", "vao", "ebo" });
            // InterleavedGraphicsData reflects BaseGraphicsData as its direct base, so the texture
            // row is registered for both texture-holding classes.
            customizations.For<BaseTextureGraphicsData>()
                .section("Graphics")
                .computed<GLuint>("texture", "Texture ID", [](BaseTextureGraphicsData& graphics)
                                  { return graphics.getTextureId(); });
            customizations.For<InterleavedGraphicsData>()
                .section("Graphics")
                .computed<GLuint>("texture", "Texture ID", [](InterleavedGraphicsData& graphics)
                                  { return graphics.getTextureId(); });
        }

        // ===================== World =====================

        void BuildCameraRow(CustomRowContext& context)
        {
            auto* name = context.row.addChildComponent<Gui::TextInput>();
            name->setReadOnly(true);
            name->setFlex(Gui::Flex::FlexWidth);

            constexpr float buttonWidth = 30.f;
            auto* change = context.row.addChildComponent<Gui::Button>(ICON_FA_SUN_O);
            change->setWidth(buttonWidth);
            auto* show = context.row.addChildComponent<Gui::Button>(ICON_FA_EYE);
            show->setWidth(buttonWidth);
            auto* reset = context.row.addChildComponent<Gui::Button>(ICON_FA_UNDO);
            reset->setWidth(buttonWidth);

            Listen(change->onClick,
                   [] { Editor::NotificationPopUp::Warning("Not implemented").show(); });
            Listen(show->onClick,
                   [owner = context.owner]
                   {
                       const auto* world = static_cast<World*>(owner());
                       if (!world || !world->currentCamera)
                       {
                           return;
                       }
                       if (auto sceneTree = GetEditor()->getWindow<SceneTreeWindowEWC>())
                       {
                           sceneTree->highlightSpecificObject(world->currentCamera);
                       }
                   });
            Listen(reset->onClick, [] { ResetCamera(); });

            context.row.setPull(
                [name, change, show, reset, owner = context.owner]
                {
                    const auto* world = static_cast<World*>(owner());
                    const BaseCamera* camera = world ? world->currentCamera : nullptr;
                    change->disableWidget(!camera);
                    show->disableWidget(!camera);
                    reset->disableWidget(!camera);
                    std::string text = camera ? camera->getComponentName().toStdString() : "";
                    if (name->getInputtedData() != text)
                    {
                        name->setInputtedData(std::move(text));
                    }
                });
        }

        void CustomizeWorld(Gui::PropertyCustomizations& customizations)
        {
            customizations.For<World>()
                .section("Global")
                .customRow("camera", "Camera", &BuildCameraRow)
                .order({ "camera" });
            customizations.For<LightningProps>().section("Lightning");
        }

        // ===================== Asset choices =====================

        void AddAssetChoices(Gui::PropertyChoices& choices)
        {
            choices.add("Atlas"_atom,
                        [](const ChoiceRequest&) -> std::vector<Core::StringAtom>
                        {
                            const auto* assets = GetAssetsManager();
                            return assets ? assets->getAtlasesAsVector()
                                          : std::vector<Core::StringAtom>{};
                        });
            choices.add("AtlasFrame"_atom,
                        [](const ChoiceRequest& request) -> std::vector<Core::StringAtom>
                        {
                            auto* assets = GetAssetsManager();
                            if (!assets || !HasAtlas(*assets, request.dependency))
                            {
                                return {};
                            }
                            return assets->getAtlas(request.dependency).getRectsAsVector();
                        });
            choices.add("Scene"_atom,
                        [](const ChoiceRequest&) -> std::vector<Core::StringAtom>
                        {
                            std::vector<Core::StringAtom> names;
                            if (const auto* scenes = GetSceneManager())
                            {
                                for (const auto& scene : scenes->getScenes())
                                {
                                    names.push_back(scene->getSceneName());
                                }
                            }
                            return names;
                        });
        }
    } // namespace

    void RegisterEditorPropertyCustomizations(Gui::PropertyCustomizations& customizations,
                                              Gui::PropertyChoices& choices,
                                              Gui::PropertyEditors& editors)
    {
        CustomizeComponents(customizations);
        CustomizeCameras(customizations);
        CustomizeRectangles(customizations, choices);
        CustomizeGraphics(customizations, editors);
        CustomizeWorld(customizations);
        AddAssetChoices(choices);
    }

    void EnsureEditorPropertyCustomizations()
    {
        static const bool registered = []
        {
            RegisterEditorPropertyCustomizations(Gui::PropertyCustomizations::Global(),
                                                 Gui::PropertyChoices::Global(),
                                                 Gui::PropertyEditors::Global());
            return true;
        }();
        std::ignore = registered;
    }
} // namespace NX
