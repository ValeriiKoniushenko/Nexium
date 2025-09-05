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
#include "Graphics/GraphicsComponents.h"

namespace Core
{

    class Transformable;
    class StaticMeshBundle;
    class BaseCamera;

    class ObjectPropertiesWindowEWC : public BaseFloatEWC
    {
        ECS_REGISTER_NEW_COMPONENT(ObjectPropertiesWindowEWC, BaseFloatEWC);

    public:
        void setTargetObject(AbstractComponent* actor);
        void resetTargetObject();

    protected:
        void onInitialize() override;
        void onDraw() override;
        void onUpdate() override;

        void tryDrawTransformable(Transformable* comp, BaseComponent* base);
        void tryDrawBaseComponent(BaseComponent* comp);
        void tryDrawStaticMeshBundle(StaticMeshBundle* comp);
        void tryDrawBaseComponentExtra(BaseComponent* comp);
        void tryDrawGraphicsComponentData(GraphicsComponentData* comp);
        void tryDrawBaseCamera(BaseCamera* comp);

    private:
        AbstractComponent* _target = nullptr;
        Repeater _slowUpdater;
        std::vector<std::pair<int, int>> _graphicsMods;
        DelegateSubscriber _onSelectChangeId;

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
