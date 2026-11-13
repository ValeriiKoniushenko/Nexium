// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/ObjectPicker.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Scene/Scene.h"
#include "NxWorld/Scene/SceneObjects/Rectangle/Rectangle.h"
#include "glm/gtc/matrix_transform.hpp"

#include "gtest/gtest.h"
#include <array>

namespace
{
    constexpr Core::ISize2 kFrameSize{ 800, 600 };

    // Minimal integration: gives the camera a render size without a window or GL context.
    class PickerViewport : public NX::ApplicationIntegration
    {
    public:
        void preInitialize() override {}
        void initialize() override {}
        void readFromCache() override {}
        void writeToCache() override {}
        void updateInput() override {}
        void tick(float) override {}
        void updateSceneInteraction(NX::Scene&) override {}
        [[nodiscard]] bool isViewportFocused() const override { return true; }
        void beforeSceneDraw() override {}
        void afterSceneDraw() override {}
        void clearSceneRenderTarget() override {}
        [[nodiscard]] Core::ISize2 getRenderSize() const override { return kFrameSize; }
    };

    // Pixel-perfect orthographic camera at z = 1000 looking down -Z: world (x, y) == pixel (x, y).
    class PickerCamera : public NX::OrthographicCamera
    {
    public:
        const glm::mat4& getMatrix() override
        {
            _cachedProjMatrix = glm::ortho(0.f, static_cast<float>(kFrameSize.width), 0.f,
                                           static_cast<float>(kFrameSize.height), 0.1f, 10000.f);
            _cachedModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -1000.f));
            _cachedCalculatedMatrix = _cachedProjMatrix * _cachedModelMatrix;
            return _cachedCalculatedMatrix;
        }
    };

    class TestPicker : public NX::RectangleBasedObjectPicker
    {
    public:
        using RectangleBasedObjectPicker::onRequest;
    };

    class ObjectPickerTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            gGameInstance = std::make_unique<NX::GameInstance>(1, _arguments.data());
            gGameInstance->setApplicationIntegration(&_viewport);
        }

        void TearDown() override
        {
            gGameInstance->setApplicationIntegration(nullptr);
            gGameInstance.reset();
        }

        NX::SceneObj::Rectangle* addRectangle(glm::vec3 position)
        {
            auto rectangle = NX::SceneObj::Rectangle::Create();
            rectangle->setPosition(NX::GPos3(position));
            rectangle->recalculateMatrices();
            auto* raw = rectangle.get();
            _scene.addObjectToScene(std::move(rectangle));
            return raw;
        }

        NX::Transformable* pick(glm::vec2 pixel)
        {
            NX::Transformable* picked = nullptr;
            _picker.requestPick([&picked](NX::Transformable* object) { picked = object; });
            _picker.onRequest(_scene, &_camera, pixel);
            return picked;
        }

        std::array<char, 13> _executableName{ "Nexium_Tests" };
        std::array<char*, 1> _arguments{ _executableName.data() };
        PickerViewport _viewport;
        PickerCamera _camera;
        NX::Scene _scene;
        TestPicker _picker;
    };

    TEST_F(ObjectPickerTests, PicksRectangleUnderCursor)
    {
        auto* rectangle = addRectangle({ 100.f, 100.f, 0.f });

        EXPECT_EQ(pick({ 150.f, 150.f }), rectangle);
        EXPECT_EQ(pick({ 50.f, 50.f }), nullptr);
        EXPECT_EQ(pick({ 250.f, 150.f }), nullptr);
    }

    TEST_F(ObjectPickerTests, PicksTopMostOfOverlappingRectangles)
    {
        // Scene objects are drawn in order with the depth test disabled, so the later object
        // is painted over the earlier one. Clicking the overlap must select what is visible.
        auto* bottom = addRectangle({ 100.f, 100.f, 0.f });
        auto* top = addRectangle({ 150.f, 150.f, 0.f });

        EXPECT_EQ(pick({ 175.f, 175.f }), top);
        EXPECT_EQ(pick({ 110.f, 110.f }), bottom);
        EXPECT_EQ(pick({ 240.f, 240.f }), top);
    }
} // namespace
