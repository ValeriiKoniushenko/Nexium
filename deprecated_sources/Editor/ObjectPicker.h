/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "Core/Color.h"
#include "Graphics/RenderTargetToTexture.h"

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
