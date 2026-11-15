// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "ObjectPropertiesWindow.h"

#include "Editor/EditorIntegration.h"
#include "Editor/EditorPropertyCustomizations.h"
#include "Editor/GuiComponents/PropertyGrid.h"
#include "Editor/IconsFontAwesome.h"
#include "Editor/ObjectSelectorManager.h"
#include "NxFundamental/ECS/BaseComponent.h"
#include "NxWorld/Framework/GameInstanceAccess.h"
#include "NxWorld/Framework/World.h"

#ifdef NEXIUM_ENABLE_3D_MODULE
    #include "Editor/GuiComponents/CheckBox.h"
    #include "Editor/GuiComponents/HorizontalLayout.h"
    #include "Editor/GuiComponents/Input.h"
    #include "Editor/GuiComponents/Label.h"
    #include "Editor/GuiComponents/Misc.h"
    #include "NxFundamental/Utils/Stringifier.h"
    #include "NxWorld/Entities/Mesh/StaticMesh.h"
    #include "NxWorld/Entities/Mesh/StaticMeshBundle.h"
#endif

using namespace Core;
using namespace NX;
using namespace NX::Gui;

#ifdef NEXIUM_ENABLE_3D_MODULE
namespace
{
    using HLayout = Gui::HorizontalLayout;

    // Row helpers of the hand-written 3D sections.

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
        if (!Verify(label))
        {
            return nullptr;
        }

        auto row = CreateHLayoutAndLabel(label, isReadOnly, size);

        auto* comp = row->addChildComponent<CheckBox>("CheckBox: {}"_f << label);
        comp->disableWidget(isReadOnly);

        return row;
    }
} // namespace
#endif

namespace NX
{
    ObjectPropertiesWindowEWC::ObjectPropertiesWindowEWC(const StringAtom& name)
        : BaseFloatEWC(componentType, name)
    {
        if (name.isEmpty())
        {
            setComponentName("Object properties"_atom);
        }
    }

    ECS_IMPL(ObjectPropertiesWindowEWC);

    void ObjectPropertiesWindowEWC::setTargetObject(AbstractComponent* actor)
    {
        _target = actor;
        if (!_grid)
        {
            return;
        }
        if (auto* component = dynamic_cast<BaseComponent*>(actor))
        {
            // Held weakly: deleting the selected object empties the panel.
            _grid->setTarget(*component);
        }
        else
        {
            _grid->clearTarget();
        }
    }

    void ObjectPropertiesWindowEWC::resetTargetObject()
    {
        _target = nullptr;
        if (_grid)
        {
            _grid->clearTarget();
        }
    }

    const char* ObjectPropertiesWindowEWC::getIcon()
    {
        return ICON_FA_EXCLAMATION_CIRCLE;
    }

    void ObjectPropertiesWindowEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();
    }

    void ObjectPropertiesWindowEWC::onOpen()
    {
        BaseFloatEWC::onOpen();

        EnsureEditorPropertyCustomizations();
        _grid = PropertyGrid::Create();
        _grid->setStableId("Properties"_atom);
        _grid->setSections(PropertySections::PerClass);
        createGui3D();

        _subscriptionPool << GetObjectSelectorManager()->onChange->subscribeAndGetID(
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

    void ObjectPropertiesWindowEWC::onClose()
    {
        BaseFloatEWC::onClose();

        _grid = nullptr;
        _staticMeshBundleLayout.removeAllChildren();
        _staticMeshLayout.removeAllChildren();
        _renderMeshesCount = _renderBundlesCount = _activeTrianglesCount = nullptr;
        _ignoreMouseSelectBundle = nullptr;
        _outlineShader = nullptr;
        _target = nullptr;
    }

    void ObjectPropertiesWindowEWC::onDraw()
    {
        if (_grid)
        {
            _grid->tick(GetWorld()->getTimeDelta());
        }

#ifdef NEXIUM_ENABLE_3D_MODULE
        tryDrawStaticMeshBundle(dynamic_cast<StaticMeshBundle*>(_target));
        tryDrawStaticMesh(dynamic_cast<StaticMesh*>(_target));
#endif
    }

    void ObjectPropertiesWindowEWC::onUpdate()
    {
        BaseFloatEWC::onUpdate();
    }

    void ObjectPropertiesWindowEWC::createGui3D()
    {
#ifdef NEXIUM_ENABLE_3D_MODULE
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

        // ================= StaticMesh ====================
        {
            auto& out = _staticMeshLayout;

            out.attachChild(::Create<TextInput>("Outline shader name", true));
            _outlineShader = out.getLastChildAs<HLayout>()->getLastChildAs<TextInput>().get();
        }
#endif
    }

#ifdef NEXIUM_ENABLE_3D_MODULE
    void ObjectPropertiesWindowEWC::tryDrawStaticMesh(StaticMesh* comp)
    {
        if (comp && Gui::CollapsingHeader("Static mesh", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const float dt = GetWorld()->getTimeDelta();

            if (_outlineShader)
            {
                std::string shader = Stringify::gcDefaultNoneString.data();
                if (comp->getOutlineShader())
                {
                    shader = comp->getOutlineShader()->getName().toStdString();
                }
                _outlineShader->setInputtedData(shader);
            }
            _staticMeshLayout.tick(dt);
        }
    }

    void ObjectPropertiesWindowEWC::tryDrawStaticMeshBundle(StaticMeshBundle* comp)
    {
        if (comp && Gui::CollapsingHeader("Static mesh bundle", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const float dt = GetWorld()->getTimeDelta();

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
#endif
} // namespace NX
