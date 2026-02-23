/*
 * MIT License
 *
 * Copyright (c) 2018-2026 Valerii Koniushenko
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

#include "ObjectPropertiesWindow.h"

#include "../../Misc/Configs.h"
#include "Camera/Camera.h"
#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/Misc.h"
#include "Editor/GuiComponents/Spacer.h"
#include "GameplaySystem/ECS/Transformable.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Graphics/Primitives/StaticMeshBundle.h"
#include "ImGui/imgui_internal.h"
#include "ObjectPropertiesWindow.generated.cpp.inl" // this line added by the code generator.

using namespace Core;
using namespace Core::Gui;

namespace
{
    using HLayout = Gui::HorizontalLayout;

    // =========================================================
    //                 GUI STANDARD TEMPLATES
    // =========================================================

    HLayout::Ptr CreateHLayoutAndLabel(const char* label, bool isReadOnly,
                                       float size = ObjectPropertiesWindowEWC::defaultLabelWidthBig)
    {
        if (!Verify(label))
        {
            return nullptr;
        }

        auto row = HLayout::Create();
        row->setComponentName("H-Layout({}): {}"_f << (isReadOnly ? "RO" : "W") << label);
        row->addChildComponent<Label>(label)->setWidth(size);

        return row;
    }

    template<class T>
        requires std::derived_from<std::remove_reference_t<T>, BaseInput>
    HLayout::Ptr Create(const char* label, bool isReadOnly,
                        float size = ObjectPropertiesWindowEWC::defaultLabelWidthBig)
    {
        if (!Verify(label))
        {
            return nullptr;
        }

        auto row = CreateHLayoutAndLabel(label, isReadOnly, size);

        auto* comp = row->addChildComponent<T>("Input: {}"_f << label);
        comp->setFlex(Flex::FlexWidth);
        comp->disableWidget(isReadOnly);

        return row;
    }

    template<class T>
        requires std::is_same_v<T, CheckBox>
    HLayout::Ptr Create(const char* label, bool isReadOnly,
                        float size = ObjectPropertiesWindowEWC::defaultLabelWidthBig)
    {
        using namespace Core;

        if (!Verify(label))
        {
            return nullptr;
        }

        auto row = CreateHLayoutAndLabel(label, isReadOnly, size);

        auto* comp = row->addChildComponent<CheckBox>("CheckBox: {}"_f << label);
        comp->disableWidget(isReadOnly);

        return row;
    }

    template<IsComponent T>
    HLayout::Ptr CreateEx(const char* label, bool isReadOnly,
                          float size = ObjectPropertiesWindowEWC::defaultLabelWidthBig)
    {
        using namespace Core;

        if (!Verify(label))
        {
            return nullptr;
        }

        auto row = CreateHLayoutAndLabel(label, isReadOnly, size);

        auto* comp = row->addChildComponent<T>("{}: {}"_f << T::componentType << label);

        return row;
    }

    // =========================================================
} // namespace

namespace Core
{
    ECS_COMPONENT_IMPL(ObjectPropertiesWindowEWC)

    Gui::HorizontalLayout::Ptr _GraphicsModifiersArray_ArrayCellViewerFunc::operator()(
        GraphicsComponentData::ModifierParam& data) const
    {
        auto l = Gui::HorizontalLayout::Create();

        {
            static const auto& modifiers = []()
            {
                const auto& m = R<GraphicsComponentData::Modifier>::ToArrayN();

                std::vector<std::string> out(m.begin(), m.end());
                std::ranges::sort(out);
                auto r = std::ranges::remove(out, "None");
                out.erase(r.begin(), r.end());
                return out;
            }();

            const auto comboModifier = l->addChildComponent<Gui::ComboModelBased>();
            comboModifier->setDataProvider(
                [](std::size_t i, StringAtom& out) -> const void*
                {
                    out = modifiers.at(i);
                    return &modifiers.at(i);
                });
            comboModifier->setSizeProvider([] { return modifiers.size(); });
            comboModifier->setFlex(Gui::Flex::FlexWidth);
            comboModifier->setCurrentIndex(static_cast<int>(data.modifier) - 1);
        }

        {
            static const auto& values = []()
            {
                const auto& m = R<GraphicsComponentData::ModifiedValue>::ToArrayN();

                std::vector<std::string> out(m.begin(), m.end());
                std::ranges::sort(out);
                auto r = std::ranges::remove(out, "None");
                out.erase(r.begin(), r.end());
                return out;
            }();

            const auto comboValues = l->addChildComponent<Gui::ComboModelBased>();
            comboValues->setDataProvider(
                [](std::size_t i, StringAtom& out) -> const void*
                {
                    out = values.at(i);
                    return &values.at(i);
                });
            comboValues->setSizeProvider([] { return values.size(); });
            comboValues->setFlex(Gui::Flex::FlexWidth);

            auto it = std::ranges::find(values, R<decltype(data.value)>::ToString(data.value));
            if (it != values.end())
            {
                comboValues->setCurrentIndex(std::distance(values.begin(), it));
            }
        }

        return l;
    }

    GraphicsComponentData::ModifierParam _GraphicsModifiersArray_ViewFetchFunc::operator()(
        Gui::HorizontalLayout* layout) const
    {
        GraphicsComponentData::ModifierParam out;

        {
            static const auto& modifiers = []()
            {
                const auto& m = R<GraphicsComponentData::Modifier>::ToArrayN();

                std::vector<std::string> out(m.begin(), m.end());
                std::ranges::sort(out);
                auto r = std::ranges::remove(out, "None");
                out.erase(r.begin(), r.end());
                return out;
            }();

            if (auto modifier = layout->getFirstChildAs<Gui::ComboModelBased>(); modifier)
            {
                const auto str = modifiers[modifier->getCurrentIndex()];
                out.modifier = R<GraphicsComponentData::Modifier>::FromString(str.c_str()).value();
            }
            else
            {
                Assert(false);
            }
        }

        {
            static const auto& values = []()
            {
                const auto& m = R<GraphicsComponentData::ModifiedValue>::ToArrayN();

                std::vector<std::string> out(m.begin(), m.end());
                std::ranges::sort(out);
                auto r = std::ranges::remove(out, "None");
                out.erase(r.begin(), r.end());
                return out;
            }();

            if (auto value = layout->getLastChildAs<Gui::ComboModelBased>(); value)
            {
                const auto str = values[value->getCurrentIndex()];
                out.value = R<GraphicsComponentData::ModifiedValue>::FromString(str).value_or(
                    GraphicsComponentData::ModifiedValue::None);
            }
            else
            {
                Assert(false);
            }
        }

        return out;
    }

    //
    //     _____  _        _              _   ______
    //    |  _  || |      (_)            | |  | ___ \
    //    | | | || |__     _   ___   ___ | |_ | |_/ / _ __   ___   _ __   ___
    //    | | | || '_ \   | | / _ \ / __|| __||  __/ | '__| / _ \ | '_ \ / __|
    //    \ \_/ /| |_) |  | ||  __/| (__ | |_ | |    | |   | (_) || |_) |\__ \
    //     \___/ |_.__/   | | \___| \___| \__|\_|    |_|    \___/ | .__/ |___/
    //                   _/ |                                     | |
    //                  |__/                                      |_|
    // ========================================================================
    void ObjectPropertiesWindowEWC::setTargetObject(AbstractComponent* actor)
    {
        _target = actor;
        setChildListData(_target);
        setGraphicsModifiers(_target);
    }

    void ObjectPropertiesWindowEWC::resetTargetObject()
    {
        _target = nullptr;
    }

    void ObjectPropertiesWindowEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        createGui();
        registerGuiEvents();

        _subscriptionPool << gGameInstance->objectSelectorManager.onChange->subscribeAndGetID(
            [this](BaseComponent* comp, bool newValue)
            {
                if (newValue)
                {
                    setTargetObject(comp);
                }
                else
                {
                    resetTargetObject();
                }
            });
    }

    void ObjectPropertiesWindowEWC::onDraw()
    {
        auto* asBaseComponent = dynamic_cast<BaseComponent*>(_target);
        auto* asTransformable = dynamic_cast<Transformable*>(_target);
        auto* asStaticMeshBundle = dynamic_cast<StaticMeshBundle*>(_target);
        auto* asGraphicsComponentData = dynamic_cast<GraphicsComponentData*>(_target);
        auto* asStaticMesh = dynamic_cast<StaticMesh*>(_target);
        auto* asBaseCamera = dynamic_cast<BaseCamera*>(_target);

        tryDrawBaseComponent(asBaseComponent);
        tryDrawTransformable(asTransformable, asBaseComponent);
        tryDrawStaticMeshBundle(asStaticMeshBundle);
        tryDrawStaticMesh(asStaticMesh);
        tryDrawGraphicsComponentData(asGraphicsComponentData);
        tryDrawBaseCamera(asBaseCamera);
        tryDrawBaseComponentExtra(asBaseComponent);
    }

    void ObjectPropertiesWindowEWC::onUpdate()
    {
        BaseFloatEWC::onUpdate();
    }

    void ObjectPropertiesWindowEWC::createGui()
    {
        // ================= General ====================
        {
            auto& out = _generalInfoLayout;

            out.attachChild(::Create<TextInput>("Name", true, defaultLabelWidth));
            _objectName = out.getLastChildAs<HLayout>()->getLastChildAs<TextInput>().get();

            out.attachChild(::Create<TextInput>("Type", true, defaultLabelWidth));
            _objectType = out.getLastChildAs<HLayout>()->getLastChildAs<TextInput>().get();

            out.attachChild(::Create<CheckBox>("Enabled", false, defaultLabelWidth));
            _objectIsEnabled = out.getLastChildAs<HLayout>()->getLastChildAs<CheckBox>().get();
        }

        // ================= StaticMeshBundle ====================
        {
            auto& out = _staticMeshBundleLayout;

            out.attachChild(::Create<IntInput>("Sub-render meshes", true));
            _renderMeshesCount = out.getLastChildAs<HLayout>()->getLastChildAs<IntInput>().get();

            out.attachChild(::Create<IntInput>("Sub-render bundles", true));
            _renderBundlesCount = out.getLastChildAs<HLayout>()->getLastChildAs<IntInput>().get();

            out.attachChild(::Create<IntInput>("Active triangles", true));
            _activeTrianglesCount = out.getLastChildAs<HLayout>()->getLastChildAs<IntInput>().get();

            out.attachChild(::Create<CheckBox>("Hidden from scene", true));
            _ignoreMouseSelectBundle
                = out.getLastChildAs<HLayout>()->getLastChildAs<CheckBox>().get();
        }

        // ================= BaseComponent-extra ====================
        {
            auto& out = _baseComponentExtraLayout;

            out.attachChild(::Create<TextInput>("Parent", true));
            _parentName = out.getLastChildAs<HLayout>()->getLastChildAs<TextInput>().get();

            out.attachChild(::Create<IntInput>("Children count", true));
            _childrenCount = out.getLastChildAs<HLayout>()->getLastChildAs<IntInput>().get();

            out.attachChild(::Create<CheckBox>("Is initialized", true));
            _isInited = out.getLastChildAs<HLayout>()->getLastChildAs<CheckBox>().get();

            out.attachChild(::Create<CheckBox>("Disabled ticks", false));
            _disabledTicks = out.getLastChildAs<HLayout>()->getLastChildAs<CheckBox>().get();

            out.attachChild(::CreateEx<StringArray>("Children", true));
            out.getLastChildAs<HLayout>()->setVerticalAlign(Align::Top);
            _childrenList = out.getLastChildAs<HLayout>()->getLastChildAs<StringArray>().get();
            _childrenList->setReadOnly(true);
        }

        // ================= StaticMesh ====================
        {
            auto& out = _staticMeshLayout;

            out.attachChild(::Create<TextInput>("Outline shader name", true));
            _outlineShader = out.getLastChildAs<HLayout>()->getLastChildAs<TextInput>().get();
        }

        // ================= GraphicsComponent ====================
        {
            auto& out = _graphicsComponentLayout;
            out.attachChild(::Create<IntInput>("Triangles", true));
            _graphicsTriangles = out.getLastChildAs<HLayout>()->getLastChildAs<IntInput>().get();

            out.attachChild(::Create<TextInput>("Shader name", true));
            _graphicsShader = out.getLastChildAs<HLayout>()->getLastChildAs<TextInput>().get();

            out.attachChild(::Create<IntInput>("VBO ID", true));
            _graphicsVBO = out.getLastChildAs<HLayout>()->getLastChildAs<IntInput>().get();

            out.attachChild(::Create<IntInput>("VAO ID", true));
            _graphicsVAO = out.getLastChildAs<HLayout>()->getLastChildAs<IntInput>().get();

            out.attachChild(::Create<IntInput>("EBO ID", true));
            _graphicsEBO = out.getLastChildAs<HLayout>()->getLastChildAs<IntInput>().get();

            out.attachChild(::Create<IntInput>("Texture ID", true));
            _graphicsTexture = out.getLastChildAs<HLayout>()->getLastChildAs<IntInput>().get();

            out.attachChild(::CreateEx<GraphicsModifiersArray>("Graphic modifier", false));
            out.getLastChildAs<HLayout>()->setVerticalAlign(Align::Top);
            _graphicsModifiers
                = out.getLastChildAs<HLayout>()->getLastChildAs<GraphicsModifiersArray>().get();
        }

        // ================= BaseCamera ====================
        {
            auto& out = _baseCameraLayout;

            out.attachChild(::Create<FloatInput>("FOV", false));
            _cameraFov = out.getLastChildAs<HLayout>()->getLastChildAs<FloatInput>().get();
            _cameraFov->setMin(BaseCamera::minFov);
            _cameraFov->setMax(BaseCamera::maxFov);
            _cameraFov->setStep(1.f);

            out.attachChild(::Create<FloatInput>("Far plane", false));
            _cameraFar = out.getLastChildAs<HLayout>()->getLastChildAs<FloatInput>().get();
            _cameraFar->setMin(100.f);
            _cameraFar->setMax(1'000'000.f);
            _cameraFar->setStep(100.f);

            out.attachChild(::Create<FloatInput>("Near plane", false));
            _cameraNear = out.getLastChildAs<HLayout>()->getLastChildAs<FloatInput>().get();
            _cameraNear->setMin(0.1f);
            _cameraNear->setMax(1000.f);
            _cameraNear->setStep(0.1f);

            out.attachChild(CreateHLayoutAndLabel("Frame size(ration)", false));
            _cameraFrame = out.getLastChildAs<HLayout>()->addChildComponent<Float2Input>();
            _cameraFrame->setLabelText({ 'W', 'H' });

            out.attachChild(CreateHLayoutAndLabel("Output size", true));
            _cameraOutput = out.getLastChildAs<HLayout>()->addChildComponent<Float2Input>();
            _cameraOutput->setLabelText({ 'W', 'H' });
            _cameraOutput->setReadOnly(true);
        }

        // ================= Transform ====================
        {
            auto& out = _transformableLayout;

            out.attachChild(CreateHLayoutAndLabel("Position", false, defaultLabelWidth));
            _transformPosition = out.getLastChildAs<HLayout>()->addChildComponent<Float3Input>();

            out.attachChild(CreateHLayoutAndLabel("Rotation", false, defaultLabelWidth));
            _transformRotation = out.getLastChildAs<HLayout>()->addChildComponent<Float3Input>();

            out.attachChild(CreateHLayoutAndLabel("Scale", false, defaultLabelWidth));
            _transformScale = out.getLastChildAs<HLayout>()->addChildComponent<Float3Input>();

            out.attachChild(CreateHLayoutAndLabel("Origin", false, defaultLabelWidth));
            _transformOrigin = out.getLastChildAs<HLayout>()->addChildComponent<Float3Input>();
        }
    }

    void ObjectPropertiesWindowEWC::registerGuiEvents()
    {
        if (_objectIsEnabled)
        {
            _subscriptionPool << _objectIsEnabled->onChange->subscribeAndGetID(
                [this](bool newStatus)
                {
                    if (_target)
                    {
                        _target->setEnabled(newStatus);
                    }
                });
        }

        if (_disabledTicks)
        {
            _subscriptionPool << _disabledTicks->onChange->subscribeAndGetID(
                [this](bool newStatus)
                {
                    if (_target)
                    {
                        _target->setNoTick(newStatus);
                    }
                });
        }

        if (_cameraFov)
        {
            _subscriptionPool << _cameraFov->onInput->subscribeAndGetID(
                [this](float newValue)
                {
                    if (auto* camera = _target->tryCastTo<BaseCamera>())
                    {
                        camera->setFov(newValue);
                    }
                });
        }

        if (_cameraFar)
        {
            _subscriptionPool << _cameraFar->onInput->subscribeAndGetID(
                [this](float newValue)
                {
                    if (auto* camera = _target->tryCastTo<BaseCamera>())
                    {
                        camera->setFar(newValue);
                    }
                });
        }

        if (_cameraNear)
        {
            _subscriptionPool << _cameraNear->onInput->subscribeAndGetID(
                [this](float newValue)
                {
                    if (auto* camera = _target->tryCastTo<BaseCamera>())
                    {
                        camera->setNear(newValue);
                    }
                });
        }
        if (_cameraFrame)
        {
            _subscriptionPool << _cameraFrame->onInput->subscribeAndGetID(
                [this](glm::vec2 newValue)
                {
                    if (auto* camera = _target->tryCastTo<BaseCamera>())
                    {
                        camera->setFrameSize(FSize2(newValue));
                    }
                });
        }
        if (_transformPosition)
        {
            _subscriptionPool << _transformPosition->onInput->subscribeAndGetID(
                [this](auto newValue)
                {
                    if (_target)
                    {
                        if (auto* trans = dynamic_cast<Transformable*>(_target))
                        {
                            trans->setPosition(GPos3(newValue));
                        }
                    }
                });
        }
        if (_transformRotation)
        {
            _subscriptionPool << _transformRotation->onInput->subscribeAndGetID(
                [this](auto newValue)
                {
                    if (_target)
                    {
                        if (auto* trans = dynamic_cast<Transformable*>(_target))
                        {
                            trans->setRotation(newValue);
                        }
                    }
                });
        }
        if (_transformScale)
        {
            _subscriptionPool << _transformScale->onInput->subscribeAndGetID(
                [this](auto newValue)
                {
                    if (_target)
                    {
                        if (auto* trans = dynamic_cast<Transformable*>(_target))
                        {
                            trans->setScale(newValue);
                        }
                    }
                });
        }
        if (_transformOrigin)
        {
            _subscriptionPool << _transformOrigin->onInput->subscribeAndGetID(
                [this](auto newValue)
                {
                    if (_target)
                    {
                        if (auto* trans = dynamic_cast<Transformable*>(_target))
                        {
                            trans->setOrigin(newValue);
                        }
                    }
                });
        }
        if (_graphicsModifiers)
        {
            _subscriptionPool << _graphicsModifiers->onSave->subscribeAndGetID(
                [this](const auto& data)
                {
                    if (!_target)
                    {
                        return;
                    }
                    if (auto* graph = dynamic_cast<GraphicsComponentData*>(_target))
                    {
                        graph->setDrawModifiers(data);
                    }
                });
            _subscriptionPool << _graphicsModifiers->onReset->subscribeAndGetID(
                [this](auto& out)
                {
                    if (!_target)
                    {
                        return;
                    }
                    if (auto* graph = dynamic_cast<GraphicsComponentData*>(_target))
                    {
                        out = graph->getDrawModifiers();
                    }
                });
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawTransformable(Transformable* comp, BaseComponent* base)
    {
        if (comp && Gui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const float dt = GetWorld().timeDelta;

            if (_transformPosition)
            {
                _transformPosition->setInputtedData(comp->getPosition());
            }

            if (_transformRotation)
            {
                _transformRotation->setInputtedData(comp->getRotation());
            }

            if (_transformScale)
            {
                _transformScale->setInputtedData(comp->getScale());
            }

            if (_transformOrigin)
            {
                _transformOrigin->setInputtedData(comp->getOrigin());
            }

            _transformableLayout.tick(dt);
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawBaseComponent(BaseComponent* comp)
    {
        if (comp && Gui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const float dt = GetWorld().timeDelta;

            if (_objectName)
            {
                _objectName->setInputtedData(comp->getComponentName().toStdString());
            }

            if (_objectType)
            {
                _objectType->setInputtedData(comp->getComponentType().toStdString());
            }

            if (_objectIsEnabled)
            {
                _objectIsEnabled->setValue(comp->isEnabled());
            }

            _generalInfoLayout.tick(dt);
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawGraphicsComponentData(GraphicsComponentData* comp)
    {
        if (comp && Gui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const float dt = GetWorld().timeDelta;

            if (_graphicsTriangles)
            {
                _graphicsTriangles->setInputtedData(comp->getTriangleCount());
            }

            if (_graphicsShader)
            {
                _graphicsShader->setInputtedData(comp->getShader()->getName().toStdString());
            }

            if (_graphicsVBO)
            {
                _graphicsVBO->setInputtedData(comp->getVboId());
            }

            if (_graphicsVAO)
            {
                _graphicsVAO->setInputtedData(comp->getVboId());
            }

            if (_graphicsEBO)
            {
                _graphicsEBO->setInputtedData(comp->getEboId());
            }

            if (_graphicsTexture)
            {
                _graphicsTexture->setInputtedData(comp->getTextureId());
            }

            _graphicsComponentLayout.tick(dt);
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawBaseComponentExtra(BaseComponent* comp)
    {
        if (comp && Gui::CollapsingHeader("Component data", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const float dt = GetWorld().timeDelta;

            if (_childrenCount)
            {
                _childrenCount->setInputtedData(static_cast<int>(comp->getChildrenCount()));
            }
            if (_isInited)
            {
                _isInited->setValue(comp->isInitialized());
            }
            if (_disabledTicks)
            {
                _disabledTicks->setValue(comp->getNoTick());
            }
            if (_parentName)
            {
                std::string str = "None";
                if (comp->hasParent())
                {
                    str = comp->getParent()->getComponentName().toStdString();
                    str += " (";
                    str += comp->getParent()->getComponentType().toStdString();
                    str += ")";
                }
                _parentName->setInputtedData(std::move(str));
            }

            _baseComponentExtraLayout.tick(dt);
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawStaticMesh(StaticMesh* comp)
    {
        if (comp && Gui::CollapsingHeader("Static mesh", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const float dt = GetWorld().timeDelta;

            if (_outlineShader)
            {
                std::string shader = "None";
                if (comp->getOutlineShader())
                {
                    shader = comp->getOutlineShader()->getName().toStdString();
                }
                _outlineShader->setInputtedData(shader);
            }
            _staticMeshLayout.tick(dt);
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawBaseCamera(BaseCamera* comp)
    {
        if (comp && Gui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const float dt = GetWorld().timeDelta;

            if (_cameraFov)
            {
                _cameraFov->setInputtedData(comp->getFov());
            }
            if (_cameraFar)
            {
                _cameraFar->setInputtedData(comp->getFar());
            }
            if (_cameraNear)
            {
                _cameraNear->setInputtedData(comp->getNear());
            }
            if (_cameraFrame)
            {
                _cameraFrame->setInputtedData(comp->getFrameSize().toGlm());
            }
            if (_cameraOutput)
            {
                _cameraOutput->setInputtedData(comp->getOutputFrameSize().toGlm());
            }

            _baseCameraLayout.tick(dt);
        }
    }

    void ObjectPropertiesWindowEWC::setChildListData(AbstractComponent* abstComp)
    {
        if (!_childrenList || !abstComp)
        {
            return;
        }

        if (auto* comp = dynamic_cast<BaseComponent*>(abstComp))
        {
            std::vector<StringAtom> childrenStrs;
            childrenStrs.reserve(comp->getChildrenCount());
            for (auto&& child : comp->getChildren())
            {
                StringAtom str = child->getComponentName();
                str += " (";
                str += child->getComponentType();
                str += ")";
                childrenStrs.push_back(std::move(str));
            }

            _childrenList->setData(std::move(childrenStrs));
        }
    }

    void ObjectPropertiesWindowEWC::setGraphicsModifiers(AbstractComponent* abstComp)
    {
        if (!_graphicsModifiers || !abstComp)
        {
            return;
        }

        if (auto* comp = dynamic_cast<GraphicsComponentData*>(abstComp))
        {
            _graphicsModifiers->setData(comp->getDrawModifiers());
        }
        else
        {
            _graphicsModifiers->setData({});
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawStaticMeshBundle(StaticMeshBundle* comp)
    {
        if (comp && Gui::CollapsingHeader("Static mesh bundle", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const float dt = GetWorld().timeDelta;

            if (_renderMeshesCount)
            {
                _renderMeshesCount->setInputtedData(static_cast<int>(comp->getRenderMeshesCount()));
            }

            if (_renderBundlesCount)
            {
                _renderBundlesCount->setInputtedData(
                    static_cast<int>(comp->getRenderBundlesCount()));
            }

            if (_ignoreMouseSelectBundle)
            {
                _ignoreMouseSelectBundle->setValue(comp->isIgnoreSelect());
            }

            if (_activeTrianglesCount)
            {
                _activeTrianglesCount->setInputtedData(
                    static_cast<int>(comp->getRenderableTriangles()));
            }

            _staticMeshBundleLayout.tick(dt);
        }
    }
} // namespace Core