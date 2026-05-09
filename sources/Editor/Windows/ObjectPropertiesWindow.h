/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#pragma once

#include "BaseWindow.h"
#include "Editor/GuiComponents/Array.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/VecInput.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "Graphics/GraphicsComponents.h"
#include "Graphics/Primitives/StaticMesh.h"

namespace Core
{
    namespace Gui
    {
        class CheckBox;
        class TextInput;
    } // namespace Gui

    class Transformable;
    class StaticMeshBundle;
    class BaseCamera;

    struct _GraphicsModifiersArray_ArrayCellViewerFunc
    {
        Gui::HorizontalLayout::Ptr operator()(InterleavedGraphicsData::ModifierParam& data) const;
    };

    struct _GraphicsModifiersArray_ViewFetchFunc
    {
        InterleavedGraphicsData::ModifierParam operator()(Gui::HorizontalLayout* layout) const;
    };

    using GraphicsModifiersArray = Gui::BaseArray<InterleavedGraphicsData::ModifierParam,
                                                  _GraphicsModifiersArray_ArrayCellViewerFunc,
                                                  _GraphicsModifiersArray_ViewFetchFunc>;

    CLASS();
    class ObjectPropertiesWindowEWC : public BaseFloatEWC
    {
        R_FRIEND_DECL(Core::ObjectPropertiesWindowEWC, Core::BaseFloatEWC);
        ECS_COMPONENT_DECL(ObjectPropertiesWindowEWC, BaseFloatEWC);

    public:
        static constexpr float defaultLabelWidth = 100.0f;
        static constexpr float defaultLabelWidthBig = 150.0f;

    public:
        void setTargetObject(AbstractComponent* actor);

        void resetTargetObject();

        [[nodiscard]] const char* getIcon() override { return ICON_FA_EXCLAMATION_CIRCLE; }

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

        // BaseCamera section:
        Gui::VerticalLayout _baseCameraLayout;
        Gui::FloatInput* _cameraFov = nullptr;
        Gui::FloatInput* _cameraFar = nullptr;
        Gui::FloatInput* _cameraNear = nullptr;
        Gui::Float2Input* _cameraFrame = nullptr;
        Gui::Float2Input* _cameraOutput = nullptr;

        // StaticMesh section:
        Gui::VerticalLayout _staticMeshLayout;
        Gui::TextInput* _outlineShader = nullptr;

        // Transformable section:
        Gui::VerticalLayout _transformableLayout;
        Gui::Float3Input* _transformPosition = nullptr;
        Gui::Float3Input* _transformRotation = nullptr;
        Gui::Float3Input* _transformScale = nullptr;
        Gui::Float3Input* _transformOrigin = nullptr;

        AbstractComponent* _target = nullptr;
    };
} // namespace Core

#include "ObjectPropertiesWindow.generated.h" // added by the code generator. Better don't move it.
