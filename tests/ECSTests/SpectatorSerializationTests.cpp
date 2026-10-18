// Nexium
// Copyright 2018-2026 Valerii Koniushenko
// Licensed under the Apache License, Version 2.0.

#include "NxWorld/Framework/Input/InputSystem.h"
#include "NxWorld/Scene/Spectator.h"

#include "gtest/gtest.h"

using namespace NX;

namespace
{
    template<class SpectatorT>
    class SpectatorSerializationTests : public ::testing::Test
    {
    protected:
        void SetUp() override { GetInputSystem().resetInput(); }
        void TearDown() override { GetInputSystem().resetInput(); }

        static nlohmann::json legacyData()
        {
            auto data = SpectatorT::Create()->serialize();
            data["speed"] = 100.f;
            data["mouseSensitivity"] = 0.17f;
            return data;
        }
    };

    using SpectatorTypes = ::testing::Types<Spectator2D, Spectator3D>;
    TYPED_TEST_SUITE(SpectatorSerializationTests, SpectatorTypes);
} // namespace

TYPED_TEST(SpectatorSerializationTests, MigratesLegacySettingsAndPreservesEditsAfterReinitializing)
{
    auto spectator = TypeParam::Create();
    RResourceStream<RJsonResourceStream> stream(this->legacyData());
    spectator->deserialize(stream);

    auto* input = spectator->getInputController();
    ASSERT_NE(input, nullptr);
    EXPECT_FLOAT_EQ(input->speed, 100.f);
    EXPECT_FLOAT_EQ(input->mouseSensitivity, 0.17f);

    const auto serialized = spectator->serialize();
    EXPECT_FALSE(serialized.contains("speed"));
    EXPECT_FALSE(serialized.contains("mouseSensitivity"));
    auto restored = TypeParam::Create();
    RResourceStream<RJsonResourceStream> roundTrip(serialized);
    restored->deserialize(roundTrip);
    ASSERT_NE(restored->getInputController(), nullptr);
    EXPECT_FLOAT_EQ(restored->getInputController()->speed, 100.f);
    EXPECT_FLOAT_EQ(restored->getInputController()->mouseSensitivity, 0.17f);

    input->speed = 145.f;
    input->mouseSensitivity = 0.31f;
    spectator->deinitialize();
    spectator->initialize();
    EXPECT_EQ(spectator->getInputController(), input);
    EXPECT_FLOAT_EQ(input->speed, 145.f);
    EXPECT_FLOAT_EQ(input->mouseSensitivity, 0.31f);
}

TYPED_TEST(SpectatorSerializationTests, ModernInputSettingsTakePrecedenceOverLegacyRootFields)
{
    auto spectator = TypeParam::Create();
    spectator->initialize();
    auto* input = spectator->getInputController();
    ASSERT_NE(input, nullptr);
    input->speed = 345.f;
    input->mouseSensitivity = 0.27f;

    auto serialized = spectator->serialize();
    serialized["speed"] = 100.f;
    serialized["mouseSensitivity"] = 0.17f;
    auto restored = TypeParam::Create();
    RResourceStream<RJsonResourceStream> stream(serialized);
    restored->deserialize(stream);
    ASSERT_NE(restored->getInputController(), nullptr);
    EXPECT_FLOAT_EQ(restored->getInputController()->speed, 345.f);
    EXPECT_FLOAT_EQ(restored->getInputController()->mouseSensitivity, 0.27f);
}

TYPED_TEST(SpectatorSerializationTests, DisabledLegacySettingsSurviveRoundTripBeforeInitialization)
{
    auto serialized = this->legacyData();
    serialized["_isEnabled"] = false;
    auto spectator = TypeParam::Create();
    RResourceStream<RJsonResourceStream> stream(serialized);
    spectator->deserialize(stream);
    EXPECT_FALSE(spectator->isInitialized());

    auto restored = TypeParam::Create();
    RResourceStream<RJsonResourceStream> roundTrip(spectator->serialize());
    restored->deserialize(roundTrip);
    EXPECT_FALSE(restored->isInitialized());
    restored->enable();
    restored->initialize();
    ASSERT_NE(restored->getInputController(), nullptr);
    EXPECT_FLOAT_EQ(restored->getInputController()->speed, 100.f);
    EXPECT_FLOAT_EQ(restored->getInputController()->mouseSensitivity, 0.17f);
}
