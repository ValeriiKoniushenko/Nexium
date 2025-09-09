// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "ObjectPropertiesWindow.h"

#include "Camera/Camera.h"
#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/Spacer.h"
#include "GameplaySystem/ECS/Transformable.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Graphics/Primitives/StaticMeshBundle.h"
#include "ImGui/imgui_internal.h"

using namespace Core;

namespace
{
    using HLayout = HorizontalLayout;

    // =========================================================
    //                 GUI STANDARD TEMPLATES
    // =========================================================

    HLayout::Ptr CreateHLayoutAndLabel(const char* label, bool isReadOnly)
    {
        if (!Verify(label))
        {
            return nullptr;
        }

        auto row = HLayout::Create();
        row->setComponentName("H-Layout({}): {}"_f << (isReadOnly ? "RO" : "W") << label);
        row->addChildComponent<Label>(label)->setWidth(
            ObjectPropertiesWindowEWC::defaultLabelWidthBig);

        return row;
    }

    template<class T>
        requires std::derived_from<T, BaseInput>
    HLayout::Ptr Create(const char* label, bool isReadOnly)
    {
        if (!Verify(label))
        {
            return nullptr;
        }

        auto row = CreateHLayoutAndLabel(label, isReadOnly);

        auto* comp = row->addChildComponent<T>("Input: {}"_f << label);
        comp->setFlex(Widget::Flex::FlexWidth);
        comp->setDisabled(isReadOnly);

        return row;
    }

    template<class T>
        requires std::is_same_v<T, CheckBox>
    HLayout::Ptr Create(const char* label, bool isReadOnly)
    {
        using namespace Core;

        if (!Verify(label))
        {
            return nullptr;
        }

        auto row = CreateHLayoutAndLabel(label, isReadOnly);

        auto* comp = row->addChildComponent<CheckBox>("CheckBox: {}"_f << label);
        comp->setDisabled(isReadOnly);

        return row;
    }
    // =========================================================

} // namespace

namespace Core
{
    ECS_REGISTER_NEW_TYPE(ObjectPropertiesWindowEWC)

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

        _transformLocationControl.components
            = { Vec3Control::Component{ .text = "X:"_atom, .color = ColorRed },
                Vec3Control::Component{ .text = "Y:"_atom, .color = ColorGreen },
                Vec3Control::Component{ .text = "Z:"_atom, .color = ColorBlue } };
        _transformLocationControl.labelWidth = _labelWidth;
        _transformLocationControl.label = "Location:";

        _transformOriginControl = _transformLocationControl;
        _transformOriginControl.label = "Origin:";

        _transformScaleControl = _transformLocationControl;
        _transformScaleControl.label = "Scale:";

        _transformRotationControl = _transformLocationControl;
        _transformRotationControl.label = "Rotation:";

        _meshSizeControl.components
            = { Vec3Control::Component{ .text = "W:"_atom, .color = ColorRed },
                Vec3Control::Component{ .text = "H:"_atom, .color = ColorGreen },
                Vec3Control::Component{ .text = "D:"_atom, .color = ColorBlue } };
        _meshSizeControl.labelWidth = _labelWidth;
        _meshSizeControl.readOnly = true;
        _meshSizeControl.label = "Size:";

        _frameSizeControl.components
            = { Vec2Control::Component{ .text = "W:"_atom, .color = ColorRed },
                Vec2Control::Component{ .text = "H:"_atom, .color = ColorGreen } };
        _frameSizeControl.labelWidth = _labelWidth;
        _frameSizeControl.label = "Frame size:";

        _modifierValueVec = GraphicsComponentData::ModifiedValueAsVector();
        _modifierVec = GraphicsComponentData::ModifierAsVector();

        _modifierValueRaw.clear();
        for (auto&& value : _modifierValueVec)
        {
            for (auto c : value)
            {
                _modifierValueRaw.push_back(c);
            }
            _modifierValueRaw.push_back('\0');
        }

        _modifierRaw.clear();
        for (auto&& value : _modifierVec)
        {
            for (auto c : value)
            {
                _modifierRaw.push_back(c);
            }
            _modifierRaw.push_back('\0');
        }

        gGameInstance->objectSelectorManager.onChange.subscribe(
            [this](BaseComponent* comp, bool newValue)
            {
                if (newValue)
                {
                    _target = comp;
                }
                else
                {
                    _target = nullptr;
                }
            });
    }

    void ObjectPropertiesWindowEWC::onDraw()
    {
        // A lot of stuff that should be centered or fitted.
        // Temporary disable it
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, _overriddenSpacing);

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

        ImGui::PopStyleVar(); // ImGuiStyleVar_ItemSpacing
    }

    void ObjectPropertiesWindowEWC::onUpdate()
    {
        BaseFloatEWC::onUpdate();

        _slowUpdater.startOrUpdate();
    }

    void ObjectPropertiesWindowEWC::createGui()
    {
        // ================= General ====================
        {
            auto& out = _generalInfoLayout;

            out.attachChild(::Create<TextInput>("Name", true));
            _objectName = out.getLastChildAs<HLayout>()->getLastChildAs<TextInput>().get();

            out.attachChild(::Create<TextInput>("Type", true));
            _objectType = out.getLastChildAs<HLayout>()->getLastChildAs<TextInput>().get();

            out.attachChild(::Create<CheckBox>("Enabled", false));
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
        }

        // Frame size
        {
            auto& out = _baseCameraLayout;

            out.attachChild(CreateHLayoutAndLabel("Frame size", false));
            auto* mainRow = out.getLastChildAs<HLayout>().get();

            auto* doubleComp = mainRow->addChildComponent<HLayout>("Camera d-comp");
            doubleComp->setHorizontalAlign(Widget::Align::SpaceBetween);

            auto* width = doubleComp->addChildComponent<HLayout>("Camera W");
            auto* w = width->addChildComponent<Label>("W:");
            w->setTextColor(Color4::From(NormColor4(ColorRed)));
            width->addChildComponent<Spacer>();
            _cameraFrameWidth = width->addChildComponent<IntInput>();
            _cameraFrameWidth->setFlex(Widget::Flex::FlexWidth);

            doubleComp->addChildComponent<Spacer>()->scaleCurrentWidth(2.f);

            auto* height = doubleComp->addChildComponent<HLayout>("Camera H");
            auto* h = height->addChildComponent<Label>("H:");
            h->setTextColor(Color4::From(NormColor4(ColorGreen)));
            height->addChildComponent<Spacer>();
            _cameraFrameHeight = height->addChildComponent<IntInput>();
            _cameraFrameHeight->setFlex(Widget::Flex::FlexWidth);
        }
        // Output size
        {
            auto& out = _baseCameraLayout;

            out.attachChild(CreateHLayoutAndLabel("Output size", true));
            auto* mainRow = out.getLastChildAs<HLayout>().get();

            auto* doubleComp = mainRow->addChildComponent<HLayout>();
            doubleComp->setHorizontalAlign(Widget::Align::SpaceBetween);

            auto* width = doubleComp->addChildComponent<HLayout>();
            auto* w = width->addChildComponent<Label>("W:");
            w->setTextColor(Color4::From(NormColor4(ColorRed)));
            width->addChildComponent<Spacer>();
            _cameraOutputWidth = width->addChildComponent<IntInput>();
            _cameraOutputWidth->setFlex(Widget::Flex::FlexWidth);
            _cameraOutputWidth->setDisabled(true);

            doubleComp->addChildComponent<Spacer>()->scaleCurrentWidth(2.f);

            auto* height = doubleComp->addChildComponent<HLayout>();
            auto* h = height->addChildComponent<Label>("H:");
            h->setTextColor(Color4::From(NormColor4(ColorGreen)));
            height->addChildComponent<Spacer>();
            _cameraOutputHeight = height->addChildComponent<IntInput>();
            _cameraOutputHeight->setFlex(Widget::Flex::FlexWidth);
            _cameraOutputHeight->setDisabled(true);
        }
    }

    void ObjectPropertiesWindowEWC::registerGuiEvents()
    {
        if (_objectIsEnabled)
        {
            _objectIsEnabled->onChange.subscribe(
                [this](auto*, bool newStatus)
                {
                    if (_target)
                    {
                        _target->setEnabled(newStatus);
                    }
                });
        }

        if (_disabledTicks)
        {
            _disabledTicks->onChange.subscribe(
                [this](auto*, bool newStatus)
                {
                    if (_target)
                    {
                        _target->setNoTick(newStatus);
                    }
                });
        }

        if (_cameraFov)
        {
            _cameraFov->onInput.subscribe(
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
            _cameraFar->onInput.subscribe(
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
            _cameraNear->onInput.subscribe(
                [this](float newValue)
                {
                    if (auto* camera = _target->tryCastTo<BaseCamera>())
                    {
                        camera->setNear(newValue);
                    }
                });
        }
        if (_cameraFrameHeight)
        {
            _cameraFrameHeight->onInput.subscribe(
                [this](float newValue)
                {
                    if (auto* camera = _target->tryCastTo<BaseCamera>())
                    {
                        auto frame = camera->getFrameSize();
                        frame.height = newValue;
                        camera->setFrameSize(frame);
                    }
                });
        }
        if (_cameraFrameWidth)
        {
            _cameraFrameWidth->onInput.subscribe(
                [this](float newValue)
                {
                    if (auto* camera = _target->tryCastTo<BaseCamera>())
                    {
                        auto frame = camera->getFrameSize();
                        frame.width = newValue;
                        camera->setFrameSize(frame);
                    }
                });
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawTransformable(Transformable* comp, BaseComponent* base)
    {
        if (comp && ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const float dt = GetWorld().timeDelta;

            _transformableLayout.tick(dt);

            bool isChanged = false;

            if (auto res
                = _transformLocationControl.drawAndProcess(comp->getPosition(), _innerSize.width))
            {
                comp->setPosition(GPos3(res.value()));
                isChanged = true;
            }
            if (auto res
                = _transformRotationControl.drawAndProcess(comp->getRotation(), _innerSize.width))
            {
                comp->setRotation(res.value());
                isChanged = true;
            }
            if (auto res
                = _transformOriginControl.drawAndProcess(comp->getOrigin(), _innerSize.width))
            {
                comp->setOrigin(res.value());
                isChanged = true;
            }
            if (auto res
                = _transformScaleControl.drawAndProcess(comp->getScale(), _innerSize.width))
            {
                comp->setScale(res.value());
                isChanged = true;
            }

            if (isChanged && base)
            {
                if (auto* bundle = base->tryCastTo<StaticMeshBundle>())
                {
                    bundle->recalculateMatrices();
                }
                if (auto* mesh = base->tryCastTo<StaticMesh>())
                {
                    if (auto* bundle = mesh->tryToGetRootBundle())
                    {
                        bundle->recalculateMatrices();
                    }
                    else
                    {
                        mesh->recalculateMatrices();
                    }
                }
            }

            ImGui::Dummy(glm::vec2(0.0f, _gapBetweenSections));
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawBaseComponent(BaseComponent* comp)
    {
        if (comp && ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
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
        if (comp && ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen))
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

            ImGui::Separator();
            FixedLabel("Modifiers:", _labelWidth);
            ImGui::Dummy(glm::vec2(0, 0));
            static const auto collection = []()
            {
                auto c = GraphicsComponentData::ModifiedValueAsVector();
                std::ranges::sort(c);
                return c;
            }();

            static const std::vector<char> modifiers = []()
            {
                std::vector<char> out;
                out.reserve(512);

                for (auto&& i : collection)
                {
                    for (char const c : i)
                    {
                        out.push_back(c);
                    }
                    out.push_back('\0');
                }
                return out;
            }();

            ImGuiStyle const& style = ImGui::GetStyle();
            const char* delButtonText = "X";
            const float gap = 8.f;
            const auto buttonSize
                = ImGui::CalcTextSize(delButtonText).x + (style.FramePadding.x * 2.f);
            const auto oneComboSize
                = (_innerSize.width - _labelWidth - buttonSize - gap * 2.f) / 2.f;

            bool isDirty = false;
            auto drawModifiers = comp->getDrawModifiers();
            decltype(drawModifiers) newModifiers;

            std::size_t i = 0;
            for (auto _objData : drawModifiers)
            {
                std::pair<GraphicsComponentData::ModifiedValue, GraphicsComponentData::Modifier>
                    objData = _objData;
                ImGui::PushID(static_cast<int>(i));

                FixedLabel((StringAtom::MakeFrom(i) + "#").c_str(), _labelWidth);

                const int originalMod = getIndexFromModifier(objData.second);
                const int originalValueMod = getIndexFromModifier(objData.first);

                int currentMod = originalMod;
                int currentValueMod = originalValueMod;
                ImGui::PushItemWidth(oneComboSize);
                VectorCombo(("##ModifierVec" + StringAtom::MakeFrom(i)).c_str(), &currentMod,
                            _modifierVec);
                ImGui::SameLine(0, gap);

                VectorCombo(("##ModifierValueVec" + StringAtom::MakeFrom(i)).c_str(),
                            &currentValueMod, _modifierValueVec);
                ImGui::PopItemWidth();

                if (originalValueMod != currentValueMod)
                {
                    auto newValue
                        = GraphicsComponentData::FromString(_modifierValueVec.at(currentValueMod));

                    objData.first = newValue;
                    isDirty = true;
                }
                if (originalMod != currentMod)
                {
                    const auto newValue = GraphicsComponentData::Modifier::fromStr(
                        _modifierVec.at(currentMod).toStdString());
                    if (Verify(newValue.has_value()))
                    {
                        objData.second = newValue.value();
                        isDirty = true;
                    }
                }

                ImGui::SameLine(0, gap);

                ImGui::PushItemWidth(buttonSize);
                if (ImGui::ButtonEx(delButtonText))
                {
                    isDirty = true;
                }
                else
                {
                    newModifiers.push_back(objData);
                }
                ImGui::PopItemWidth();

                ImGui::PopID();
                ++i;
            }

            ImGui::PushItemWidth(_labelWidth);
            if (ImGui::ButtonEx("Add new item"))
            {
                newModifiers.emplace_back(GraphicsComponentData::ModifiedValue::MV_None,
                                          GraphicsComponentData::Modifier::Disable);
                isDirty = true;
            }
            ImGui::PopItemWidth();

            if (isDirty)
            {
                comp->setDrawModifiers(std::move(newModifiers));
            }

            ImGui::Dummy(glm::vec2(0.0f, _gapBetweenSections));
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawBaseComponentExtra(BaseComponent* comp)
    {
        if (comp && ImGui::CollapsingHeader("Component data", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const float dt = GetWorld().timeDelta;

            if (_childrenCount)
            {
                _childrenCount->setInputtedData(comp->getChildrenCount());
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
                    str += "(";
                    str += comp->getParent()->getComponentType().toStdString();
                    str += ")";
                }
                _parentName->setInputtedData(std::move(str));
            }

            _baseComponentExtraLayout.tick(dt);

            ImGui::Dummy(glm::vec2(0.0f, _gapBetweenSections));
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawStaticMesh(StaticMesh* comp)
    {
        if (comp && ImGui::CollapsingHeader("Static mesh", ImGuiTreeNodeFlags_DefaultOpen))
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
        if (comp && ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
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
            if (_cameraFrameHeight)
            {
                _cameraFrameHeight->setInputtedData(comp->getFrameSize().height);
            }
            if (_cameraFrameWidth)
            {
                _cameraFrameWidth->setInputtedData(comp->getFrameSize().width);
            }
            if (_cameraOutputWidth)
            {
                _cameraOutputWidth->setInputtedData(comp->getOutputFrameSize().height);
            }
            if (_cameraOutputHeight)
            {
                _cameraOutputHeight->setInputtedData(comp->getOutputFrameSize().width);
            }

            _baseCameraLayout.tick(dt);
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawStaticMeshBundle(StaticMeshBundle* comp)
    {
        if (comp && ImGui::CollapsingHeader("Static mesh bundle", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const float dt = GetWorld().timeDelta;

            if (_renderMeshesCount)
            {
                _renderMeshesCount->setInputtedData(comp->getRenderMeshesCount());
            }

            if (_renderBundlesCount)
            {
                _renderBundlesCount->setInputtedData(comp->getRenderBundlesCount());
            }

            if (_ignoreMouseSelectBundle)
            {
                _ignoreMouseSelectBundle->setValue(comp->isIgnoreSelect());
            }

            if (_activeTrianglesCount)
            {
                _activeTrianglesCount->setInputtedData(comp->getRenderableTriangles());
            }

            _staticMeshBundleLayout.tick(dt);
        }
    }

    int ObjectPropertiesWindowEWC::getIndexFromModifier(
        GraphicsComponentData::ModifiedValue v) const
    {
        for (std::size_t i = 0; i < _modifierValueVec.size(); ++i)
        {
            if (GraphicsComponentData::ToString(v) == _modifierValueVec.at(i))
            {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    int ObjectPropertiesWindowEWC::getIndexFromModifier(GraphicsComponentData::Modifier v) const
    {
        for (std::size_t i = 0; i < _modifierVec.size(); ++i)
        {
            if (v.toStr() == _modifierVec.at(i))
            {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

} // namespace Core
