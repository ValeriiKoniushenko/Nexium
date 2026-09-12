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

namespace Core
{
    class BaseCamera;
    class Scene;
    class StaticMesh;
    class StaticMeshBundle;
    class Transformable;
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

        void requestPick(const std::function<void(Transformable*)>& callback);
        void update(Scene& scene);

    protected:
        virtual void onRequest(Scene& scene, BaseCamera* camera, glm::vec2 pickPos) = 0;
        [[nodiscard]] glm::vec2 getPickedObject(const GameViewportEWC* wnd);

    protected:
        std::function<void(Transformable*)> _callback;

    private:
        bool _requested = false;
    };

    /// yes, it's slow but convenient way to pick up an object with
    /// pixel accuracy. Only for editor's aims.
    /// Also, now, it works only with StaticMeshBundle
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
        void onRequest(Scene& scene, BaseCamera* camera, glm::vec2 pickPos) override;

    private:
        std::optional<Color3> drawingPreparations(Scene& scene, BaseCamera* camera,
                                                  glm::vec2 pickPos);
        void pickingUpTheObjectBasedOnColor(Scene& scene, Color3 pickedColor);

    private:
        RenderTargetToTexture _canvas;
    };

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
        void onRequest(Scene& scene, BaseCamera* camera, glm::vec2 pickPos) override;
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

        void update(Scene& scene);
        void requestPick(const std::function<void(Transformable*)>& callback);

    private:
        std::vector<std::unique_ptr<BaseObjectPicker>> _pickers;
        std::function<void(Transformable*)> _callback;
        bool _requested = false;
    };

} // namespace Core
