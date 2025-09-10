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

#pragma once

#include "BaseWindow.h"
#include "Core/Timer.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "Graphics/GraphicsComponents.h"
#include "Graphics/Primitives/StaticMesh.h"

namespace Core
{
    class HorizontalLayout;
}
namespace Core
{
    class CheckBox;
    class TextInput;

    class Transformable;
    class StaticMeshBundle;
    class BaseCamera;

    class ObjectPropertiesWindowEWC : public BaseFloatEWC
    {
        ECS_COMPONENT_DECL(ObjectPropertiesWindowEWC, BaseFloatEWC);

    public:
        static constexpr float defaultLabelWidth = 100.0f;
        static constexpr float defaultLabelWidthBig = 150.0f;

    public:
        void setTargetObject(AbstractComponent* actor);
        void resetTargetObject();

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
        void tryDrawGraphicsComponentData(GraphicsComponentData* comp);
        void tryDrawBaseCamera(BaseCamera* comp);

    private:
        // ============= GUI =============
        // General section:
        VerticalLayout _generalInfoLayout;
        TextInput* _objectName = nullptr;
        TextInput* _objectType = nullptr;
        CheckBox* _objectIsEnabled = nullptr;

        // StaticMeshBundle section:
        VerticalLayout _staticMeshBundleLayout;
        IntInput* _renderMeshesCount = nullptr;
        IntInput* _renderBundlesCount = nullptr;
        IntInput* _activeTrianglesCount = nullptr;
        CheckBox* _ignoreMouseSelectBundle = nullptr;

        // BaseComponent-extra section:
        VerticalLayout _baseComponentExtraLayout;
        TextInput* _parentName = nullptr;
        IntInput* _childrenCount = nullptr;
        CheckBox* _isInited = nullptr;
        CheckBox* _disabledTicks = nullptr;

        // GraphicsComponent section:
        VerticalLayout _graphicsComponentLayout;
        IntInput* _graphicsTriangles = nullptr;
        TextInput* _graphicsShader = nullptr;
        IntInput* _graphicsVBO = nullptr;
        IntInput* _graphicsVAO = nullptr;
        IntInput* _graphicsEBO = nullptr;
        IntInput* _graphicsTexture = nullptr;

        // BaseCamera section:
        VerticalLayout _baseCameraLayout;
        FloatInput* _cameraFov = nullptr;
        FloatInput* _cameraFar = nullptr;
        FloatInput* _cameraNear = nullptr;
        IntInput* _cameraFrameWidth = nullptr;
        IntInput* _cameraFrameHeight = nullptr;
        IntInput* _cameraOutputWidth = nullptr;
        IntInput* _cameraOutputHeight = nullptr;

        // StaticMesh section:
        VerticalLayout _staticMeshLayout;
        TextInput* _outlineShader = nullptr;

        // Transformable section:
        VerticalLayout _transformableLayout;


        AbstractComponent* _target = nullptr;
        Repeater _slowUpdater;
        std::vector<std::pair<int, int>> _graphicsMods;

        // For transform
        Vec3Control _transformLocationControl;
        Vec3Control _transformOriginControl;
        Vec3Control _transformScaleControl;
        Vec3Control _transformRotationControl;

        // For mesh
        Vec3Control _meshSizeControl;

        // For camera
        Vec2Control _frameSizeControl;

        // For Graphics
        std::vector<StringAtom> _modifierValueVec;
        std::vector<StringAtom> _modifierVec;
        std::vector<char> _modifierValueRaw;
        std::vector<char> _modifierRaw;

        // Base settings
        float _labelWidth = 90.f;
        const glm::vec2 _overriddenSpacing = glm::vec2(0, 6);
        const float _gapBetweenSections = 15.f;

    private:
        [[nodiscard]] int getIndexFromModifier(GraphicsComponentData::ModifiedValue v) const;
        [[nodiscard]] int getIndexFromModifier(GraphicsComponentData::Modifier v) const;
    };

} // namespace Core
