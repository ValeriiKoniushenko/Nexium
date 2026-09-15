// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Color.h"
#include "RawBackend/Graphics/RenderTargetToTexture.h"

#include <memory>

namespace NX
{
    class BaseCamera;
    class Scene;
    class StaticMesh;
    class StaticMeshBundle;
    class Transformable;
} // namespace NX

namespace NX
{
    class GameViewportEWC;

    class BaseObjectPicker
    {
    public:
        BaseObjectPicker() = default;
        BaseObjectPicker(const BaseObjectPicker&) = delete;
        BaseObjectPicker& operator=(const BaseObjectPicker&) = delete;
        BaseObjectPicker(BaseObjectPicker&&) = delete;
        BaseObjectPicker& operator=(BaseObjectPicker&&) = delete;
        virtual ~BaseObjectPicker() = default;

        void requestPick(const std::function<void(NX::Transformable*)>& callback);
        void update(NX::Scene& scene);

    protected:
        virtual void onRequest(NX::Scene& scene, NX::BaseCamera* camera, glm::vec2 pickPos) = 0;
        [[nodiscard]] glm::vec2 getPickedObject(const GameViewportEWC* wnd);

    protected:
        std::function<void(NX::Transformable*)> _callback;

    private:
        bool _requested = false;
    };

#ifdef NEXIUM_ENABLE_3D_MODULE
    class SlowObjectPicker : public BaseObjectPicker
    {
    public:
        SlowObjectPicker() = default;
        SlowObjectPicker(const SlowObjectPicker&) = delete;
        SlowObjectPicker& operator=(const SlowObjectPicker&) = delete;
        SlowObjectPicker(SlowObjectPicker&&) = delete;
        SlowObjectPicker& operator=(SlowObjectPicker&&) = delete;
        ~SlowObjectPicker() override = default;

    protected:
        void onRequest(NX::Scene& scene, NX::BaseCamera* camera, glm::vec2 pickPos) override;

    private:
        std::optional<Color3> drawingPreparations(NX::Scene& scene, NX::BaseCamera* camera,
                                                  glm::vec2 pickPos);
        void pickingUpTheObjectBasedOnColor(NX::Scene& scene, Color3 pickedColor);

    private:
        NX::RenderTargetToTexture _canvas;
    };
#endif

    class RectangleBasedObjectPicker : public BaseObjectPicker
    {
    public:
        RectangleBasedObjectPicker() = default;
        RectangleBasedObjectPicker(const RectangleBasedObjectPicker&) = delete;
        RectangleBasedObjectPicker& operator=(const RectangleBasedObjectPicker&) = delete;
        RectangleBasedObjectPicker(RectangleBasedObjectPicker&&) = delete;
        RectangleBasedObjectPicker& operator=(RectangleBasedObjectPicker&&) = delete;
        ~RectangleBasedObjectPicker() override = default;

    protected:
        void onRequest(NX::Scene& scene, NX::BaseCamera* camera, glm::vec2 pickPos) override;
    };

    class ObjectPickerAggregator final
    {
    public:
        ObjectPickerAggregator();
        ObjectPickerAggregator(const ObjectPickerAggregator&) = delete;
        ObjectPickerAggregator& operator=(const ObjectPickerAggregator&) = delete;
        ObjectPickerAggregator(ObjectPickerAggregator&&) = delete;
        ObjectPickerAggregator& operator=(ObjectPickerAggregator&&) = delete;
        ~ObjectPickerAggregator() = default;

        void update(NX::Scene& scene);
        void requestPick(const std::function<void(NX::Transformable*)>& callback);

    private:
        std::vector<std::unique_ptr<BaseObjectPicker>> _pickers;
        std::function<void(NX::Transformable*)> _callback;
        bool _requested = false;
    };

} // namespace NX
