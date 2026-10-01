// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/Windows/ECSAdapters/Animation/ECSEditorFrameByFrameAnimationAdapter.h"
#include "Editor/Windows/ECSAdapters/BaseComponentAdapter.h"
#include "Editor/Windows/ECSAdapters/Input/ECSEditorInputControllerAdapter.h"
#include "NxWorld/Framework/InputController.h"

#include "gtest/gtest.h"

namespace
{
    using namespace NX;

    static_assert(RHasOnPostDeserialize<NxECSBasedEditorEWC>);

    class TestECSBasedEditor : public NxECSBasedEditorEWC
    {
    public:
        Core::StringAtom getCacheHash() const override { return "ECSBasedEditorTests"_atom; }

        void selectComponent(const BaseComponent::Ptr& component) { _targetComponent = component; }
    };

    class ECSBasedEditorTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            ImGui::CreateContext();
            ImGui::GetIO().IniFilename = nullptr;
        }

        void TearDown() override { ImGui::DestroyContext(); }
    };

    TEST_F(ECSBasedEditorTests, OldCacheRestoresParentsAndAddsMissingInputAdapter)
    {
        TestECSBasedEditor editor;
        editor.initialize();
        const auto expectedCount = editor.getChildrenCount();

        RResourceStream<RJsonResourceStream> cache;
        cache.getData() = editor.serialize();
        auto& children = cache.getData()["_children"];
        for (auto it = children.begin(); it != children.end();)
        {
            if ((*it)["_type"] == ECSEditorInputControllerAdapter::componentType.toStdString())
            {
                it = children.erase(it);
            }
            else
            {
                ++it;
            }
        }
        ASSERT_EQ(children.size() + 1, expectedCount);

        // Reload an already initialized window, as when restoring its cached state.
        for (int reload = 0; reload < 2; ++reload)
        {
            editor.deserialize(cache);
            EXPECT_EQ(editor.getChildrenCount(), expectedCount);
            for (const auto& child : editor.getChildren())
            {
                ASSERT_EQ(child->getParentAs<NxECSBasedEditorEWC>(), &editor);
                EXPECT_TRUE(child->isInitialized());
            }

            auto* input = editor.findFirstChildOf<ECSEditorInputControllerAdapter>();
            ASSERT_NE(input, nullptr);
            EXPECT_NE(editor.findFirstChildOf<ECSEditorFrameByFrameAnimationAdapter>(), nullptr);

            InputController::Ptr controller = new InputController;
            editor.selectComponent(controller);
            EXPECT_TRUE(input->canWorkWith(controller.get()));
            auto* base = editor.findFirstChildOf<ECSBaseComponentAdapter>();
            ASSERT_NE(base, nullptr);
            base->applyAssetRawData(nlohmann::json::object());
        }
    }
} // namespace
