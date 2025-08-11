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
#include "GameplaySystem/ECS/Transformable.h"
#include "Graphics/Primitives/StaticMeshBundle.h"
#include "ImGui/imgui_internal.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(ObjectPropertiesWindowEWC)

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

    void ObjectPropertiesWindowEWC::onInit()
    {
        BaseFloatEWC::onInit();

        _transformLocationControl.components = { Vec3Control::Component{ "X:"_atom, ColorRed },
                                                 Vec3Control::Component{ "Y:"_atom, ColorGreen },
                                                 Vec3Control::Component{ "Z:"_atom, ColorBlue } };
        _transformLocationControl.labelWidth = _labelWidth;
        _transformLocationControl.label = "Location:";

        _transformOriginControl = _transformLocationControl;
        _transformOriginControl.label = "Origin:";

        _transformScaleControl = _transformLocationControl;
        _transformScaleControl.label = "Scale:";

        _transformRotationControl = _transformLocationControl;
        _transformRotationControl.label = "Rotation:";

        _meshSizeControl.components = { Vec3Control::Component{ "W:"_atom, ColorRed },
                                        Vec3Control::Component{ "H:"_atom, ColorGreen },
                                        Vec3Control::Component{ "D:"_atom, ColorBlue } };
        _meshSizeControl.labelWidth = _labelWidth;
        _meshSizeControl.readOnly = true;
        _meshSizeControl.label = "Size:";

        _frameSizeControl.components = { Vec2Control::Component{ "W:"_atom, ColorRed },
                                         Vec2Control::Component{ "H:"_atom, ColorGreen } };
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
        auto* asBaseCamera = dynamic_cast<BaseCamera*>(_target);

        tryDrawBaseComponent(asBaseComponent);
        tryDrawTransformable(asTransformable);
        tryDrawGraphicsComponentData(asGraphicsComponentData);
        tryDrawStaticMeshBundle(asStaticMeshBundle);
        tryDrawBaseCamera(asBaseCamera);
        tryDrawBaseComponentExtra(asBaseComponent);

        ImGui::PopStyleVar(); // ImGuiStyleVar_ItemSpacing
    }

    void ObjectPropertiesWindowEWC::onUpdate()
    {
        BaseFloatEWC::onUpdate();

        _slowUpdater.startOrUpdate();
    }

    void ObjectPropertiesWindowEWC::tryDrawTransformable(Transformable* comp)
    {
        if (comp && ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (auto res
                = _transformLocationControl.drawAndProcess(comp->getPosition(), _innerSize.width))
            {
                comp->setPosition(GPos3(res.value()));
            }
            if (auto res
                = _transformRotationControl.drawAndProcess(comp->getRotation(), _innerSize.width))
            {
                comp->setRotation(res.value());
            }
            if (auto res
                = _transformOriginControl.drawAndProcess(comp->getOrigin(), _innerSize.width))
            {
                comp->setOrigin(res.value());
            }
            if (auto res
                = _transformScaleControl.drawAndProcess(comp->getScale(), _innerSize.width))
            {
                comp->setScale(res.value());
            }

            ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawBaseComponent(BaseComponent* comp)
    {
        if (comp && ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
        {
            LabelAndInputTextRO("Name:", comp->getComponentName(), _labelWidth, _innerSize.width);

            LabelAndInputTextRO("Type:", comp->getComponentType(), _labelWidth, _innerSize.width);

            bool isEnabled = comp->isEnabled();
            FixedLabel("Enabled:", _labelWidth);
            ImGui::Checkbox("##isEnabled", &isEnabled);

            if (isEnabled != comp->isEnabled())
            {
                comp->setEnabled(isEnabled);
            }

            ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawGraphicsComponentData(GraphicsComponentData* comp)
    {
        if (comp && ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            LabelAndInputTextRO("Triangles:", StringAtom::MakeFrom(comp->getTriangleCount()),
                                _labelWidth, _innerSize.width);

            auto shaderName = ""_atom;
            if (comp->getShaderId())
            {
                shaderName = comp->getShaderId()->getName();
            }
            LabelAndInputTextRO("Shader: ", std::move(shaderName), _labelWidth, _innerSize.width);

            if (auto* asStaticMesh = dynamic_cast<StaticMesh*>(_target))
            {
                _meshSizeControl.drawAndProcess(asStaticMesh->getSize().toGlm(), _innerSize.width);
            }

            ImGui::Separator();
            FixedLabel("Modifiers:", _labelWidth);
            ImGui::Dummy(ImVec2(0, 0));
            static const auto collection = []()
            {
                auto c = GraphicsComponentData::ModifiedValueAsVector();
                std::sort(c.begin(), c.end());
                return c;
            }();

            static const std::vector<char> modifiers = []()
            {
                std::vector<char> out;
                out.reserve(512);

                for (auto&& i : collection)
                {
                    for (char c : i)
                    {
                        out.push_back(c);
                    }
                    out.push_back('\0');
                }
                return out;
            }();

            ImGuiStyle& style = ImGui::GetStyle();
            const char* delButtonText = "X";
            const float gap = 8.f;
            const auto buttonSize
                = ImGui::CalcTextSize(delButtonText).x + style.FramePadding.x * 2.f;
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

            ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawBaseComponentExtra(BaseComponent* comp)
    {
        if (comp && ImGui::CollapsingHeader("Component data", ImGuiTreeNodeFlags_DefaultOpen))
        {
            LabelAndInputTextRO("Children:", comp->getChildrenCount(), _labelWidth,
                                _innerSize.width);
            LabelAndCheckboxRO("Inited:", comp->isInited(), _labelWidth);

            bool tickable = comp->getNoTick();
            FixedLabel("No ticks:", _labelWidth);
            ImGui::Checkbox("##NoTick", &tickable);
            if (comp->getNoTick() != tickable)
            {
                comp->setNoTick(tickable);
            }

            ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawBaseCamera(BaseCamera* comp)
    {
        if (comp && ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            float inputedFov = comp->getFov();
            LabelAndInputFloat("FOV:", inputedFov, _labelWidth, _innerSize.width, 0.1f);
            if (inputedFov != comp->getFov())
            {
                comp->setFov(inputedFov);
            }

            float inputedFar = comp->getFar();
            LabelAndInputFloat("Far:", inputedFar, _labelWidth, _innerSize.width, 1.f, 0.0001f);
            if (inputedFar != comp->getFar())
            {
                comp->setFar(inputedFar);
            }

            float inputedNear = comp->getNear();
            LabelAndInputFloat("Near:", inputedNear, _labelWidth, _innerSize.width, 0.1f, 0.0001f);
            if (inputedNear != comp->getNear())
            {
                comp->setNear(inputedNear);
            }

            if (auto res
                = _frameSizeControl.drawAndProcess(comp->getFrameSize().toGlm(), _innerSize.width))
            {
                comp->setFrameSize(FSize2(res.value()));
            }

            ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawStaticMeshBundle(StaticMeshBundle* comp)
    {
        if (comp && ImGui::CollapsingHeader("Static mesh bundle", ImGuiTreeNodeFlags_DefaultOpen))
        {
            LabelAndInputTextRO("Sub-render:", comp->getRenderTargetsCount(), _labelWidth,
                                _innerSize.width);

            ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));
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
