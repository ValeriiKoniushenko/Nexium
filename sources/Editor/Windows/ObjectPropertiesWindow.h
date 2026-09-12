// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BaseWindow.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "NxSubsystems/Graphics/GraphicsComponents.h"

namespace Core
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

    namespace SceneObj
    {
        class RectangleAnimated;
    } // namespace SceneObj

    class Transformable;
    class StaticMeshBundle;
    class StaticMesh;
    class BaseCamera;
    class PerspectiveCamera;

    struct _GraphicsModifiersArray_ArrayCellViewerFunc
    {
        Gui::HorizontalLayout::Ptr operator()(BaseGraphicsData::ModifierParam& data) const;
    };

    struct _GraphicsModifiersArray_ViewFetchFunc
    {
        BaseGraphicsData::ModifierParam operator()(Gui::HorizontalLayout* layout) const;
    };

    using GraphicsModifiersArray = Gui::BaseArray<BaseGraphicsData::ModifierParam,
                                                  _GraphicsModifiersArray_ArrayCellViewerFunc,
                                                  _GraphicsModifiersArray_ViewFetchFunc>;

    CLASS();
    class ObjectPropertiesWindowEWC : public BaseFloatEWC
    {
        ECS_DECL(ObjectPropertiesWindowEWC, Core::BaseFloatEWC);

    public:
        static constexpr float defaultLabelWidth = 100.0f;
        static constexpr float defaultLabelWidthBig = 150.0f;

    public:
        void setTargetObject(AbstractComponent* actor);

        void resetTargetObject();

        [[nodiscard]] const char* getIcon() override;

    protected:
        void onInitialize() override;

        void onDraw() override;

        void onUpdate() override;

        void createGui();

        void registerGuiEvents();

        void tryDrawBaseComponent(BaseComponent* comp);

        void tryDrawTransformable(Transformable* comp, BaseComponent* base);

        void tryDrawStaticMeshBundle(StaticMeshBundle* comp);

        void tryDrawBaseComponentExtra(BaseComponent* comp);

        void tryDrawStaticMesh(StaticMesh* static_mesh);

        void tryDrawInterleavedGraphicsData(InterleavedGraphicsData* comp);

        void tryDrawBaseCamera(BaseCamera* comp);

        void tryDrawRectangleComponent(SceneObj::RectangleAnimated* comp);

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

        void setChildListData(AbstractComponent* comp);

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

        void setGraphicsModifiers(AbstractComponent* comp);

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

        AbstractComponent* _target = nullptr;
    };
} // namespace Core

#include "ObjectPropertiesWindow.generated.h" // added by the code generator. Better don't move it.
