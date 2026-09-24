// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Editor

#pragma once

#include "BaseWindow.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "NxSubsystems/Graphics/GraphicsComponents.h"

namespace NX
{
    class AbstractComponent;
    class BaseCamera;
    class BaseComponent;
    class PerspectiveCamera;
    class StaticMesh;
    class StaticMeshBundle;
    class Transformable;

    namespace SceneObj
    {
        class RectangleAnimated;
    }
} // namespace NX

namespace NX
{
    namespace Gui
    {
        class ComboModelBased;
        class CheckBox;
        class TextInput;

        template<Utils::IsArithmetic T>
        class NumInput;

        using IntInput = NumInput<int>;
        using FloatInput = NumInput<float>;

        template<class T, class ArrayCellViewerFunc, class ViewFetchFunc>
        class BaseArray;

        struct _StringArray_ArrayCellViewerFunc;
        struct _StringArray_ViewFetchFunc;

        using StringArray
            = BaseArray<StringAtom, _StringArray_ArrayCellViewerFunc, _StringArray_ViewFetchFunc>;

        template<std::size_t Size, Utils::IsArithmetic Type>
        class VecNumInput;

        using Float3Input = VecNumInput<3, float>;
        using Float2Input = VecNumInput<2, float>;

    } // namespace Gui

    struct _GraphicsModifiersArray_ArrayCellViewerFunc
    {
        Gui::HorizontalLayout::Ptr operator()(NX::BaseGraphicsData::ModifierParam& data) const;
    };

    struct _GraphicsModifiersArray_ViewFetchFunc
    {
        NX::BaseGraphicsData::ModifierParam operator()(Gui::HorizontalLayout* layout) const;
    };

    using GraphicsModifiersArray = Gui::BaseArray<NX::BaseGraphicsData::ModifierParam,
                                                  _GraphicsModifiersArray_ArrayCellViewerFunc,
                                                  _GraphicsModifiersArray_ViewFetchFunc>;

    CLASS();
    class ObjectPropertiesWindowEWC : public BaseFloatEWC
    {
        ECS_DECL(ObjectPropertiesWindowEWC, NX::BaseFloatEWC);

    public:
        static constexpr float defaultLabelWidth = 100.0f;
        static constexpr float defaultLabelWidthBig = 150.0f;

    public:
        void setTargetObject(NX::AbstractComponent* actor);

        void resetTargetObject();

        [[nodiscard]] const char* getIcon() override;

    protected:
        void onInitialize() override;

        void onDraw() override;

        void onUpdate() override;

        void createGui();

        void registerGuiEvents();

        void tryDrawBaseComponent(NX::BaseComponent* comp);

        void tryDrawTransformable(NX::Transformable* comp, NX::BaseComponent* base);

        void tryDrawStaticMeshBundle(NX::StaticMeshBundle* comp);

        void tryDrawBaseComponentExtra(NX::BaseComponent* comp);

        void tryDrawStaticMesh(NX::StaticMesh* static_mesh);

        void tryDrawInterleavedGraphicsData(NX::InterleavedGraphicsData* comp);

        void tryDrawBaseCamera(NX::BaseCamera* comp);

        void tryDrawRectangleComponent(NX::SceneObj::RectangleAnimated* comp);

    private:
        DelegateSubscriberPoolGuard _subscriptionPool;

        // ============= GUI =============
        // General section:
        Gui::VerticalLayout _generalInfoLayout;
        Gui::TextInput* _objectName = nullptr;
        Gui::TextInput* _objectType = nullptr;
        Gui::CheckBox* _objectIsEnabled = nullptr;

        // StaticMeshBundle section:
        Gui::VerticalLayout _staticMeshBundleLayout;
        Gui::IntInput* _renderMeshesCount = nullptr;
        Gui::IntInput* _renderBundlesCount = nullptr;
        Gui::IntInput* _activeTrianglesCount = nullptr;
        Gui::CheckBox* _ignoreMouseSelectBundle = nullptr;

        // BaseComponent-extra section:
        Gui::VerticalLayout _baseComponentExtraLayout;
        Gui::TextInput* _parentName = nullptr;
        Gui::IntInput* _childrenCount = nullptr;
        Gui::StringArray* _childrenList = nullptr;

        void setChildListData(NX::AbstractComponent* comp);

        Gui::CheckBox* _isInited = nullptr;
        Gui::CheckBox* _disabledTicks = nullptr;

        // GraphicsComponent section:
        Gui::VerticalLayout _graphicsComponentLayout;
        Gui::IntInput* _graphicsTriangles = nullptr;
        Gui::TextInput* _graphicsShader = nullptr;
        Gui::IntInput* _graphicsVBO = nullptr;
        Gui::IntInput* _graphicsVAO = nullptr;
        Gui::IntInput* _graphicsEBO = nullptr;
        Gui::IntInput* _graphicsTexture = nullptr;
        GraphicsModifiersArray* _graphicsModifiers = nullptr;

        void setGraphicsModifiers(NX::AbstractComponent* comp);

        // PerspectiveCamera section:
        Gui::VerticalLayout _perspectiveCameraLayout;
        Gui::FloatInput* _cameraFov = nullptr;
        Gui::Float2Input* _cameraFrame = nullptr;
        Gui::Float2Input* _cameraOutput = nullptr;

        // OrthoCamera section:
        Gui::VerticalLayout _orthoCameraLayout;
        Gui::Float2Input* _orthoCameraLeftTop = nullptr;
        Gui::Float2Input* _orthoCameraRightBottom = nullptr;

        // BaseCamera section:
        Gui::VerticalLayout _baseCameraLayout;
        Gui::FloatInput* _cameraFar = nullptr;
        Gui::FloatInput* _cameraNear = nullptr;

        // StaticMesh section:
        Gui::VerticalLayout _staticMeshLayout;
        Gui::TextInput* _outlineShader = nullptr;

        // Transformable section:
        Gui::VerticalLayout _transformableLayout;
        Gui::Float3Input* _transformPosition = nullptr;
        Gui::Float3Input* _transformRotation = nullptr;
        Gui::Float3Input* _transformScale = nullptr;
        Gui::Float3Input* _transformOrigin = nullptr;

        // SceneObj::Rectangle section:
        Gui::VerticalLayout _rectLayout;
        Gui::ComboModelBased* _rectComboAtlas = nullptr;
        Gui::ComboModelBased* _rectComboRect = nullptr;
        Gui::CheckBox* _rectBlending = nullptr;
        Gui::CheckBox* _activateAnimation = nullptr;
        Gui::HorizontalLayout* _rectAnimationRow = nullptr;
        Gui::ComboModelBased* _rectComboAnimation = nullptr;
        Gui::HorizontalLayout* _rectAnimationFPSRow = nullptr;
        Gui::FloatInput* _rectAnimationFPS = nullptr;

        NX::AbstractComponent* _target = nullptr;
    };
} // namespace NX

#include "ObjectPropertiesWindow.generated.h" // added by the code generator. Better don't move it.
