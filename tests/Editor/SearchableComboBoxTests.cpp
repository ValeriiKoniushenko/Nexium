// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/GuiComponents/Combo.h"

#include "gtest/gtest.h"

namespace
{
    class TestSearchableComboBox : public NX::Gui::SearchableComboBox
    {
    public:
        using SearchableComboBox::matchesItem;
    };

    TEST(SearchableComboBox, MatchesSubstringWithoutCase)
    {
        TestSearchableComboBox combo;
        EXPECT_TRUE(combo.matchesItem("MenuScene"_atom));
        combo.setSearchText("uSc");
        EXPECT_TRUE(combo.matchesItem("MenuScene"_atom));
        EXPECT_FALSE(combo.matchesItem("GameScene"_atom));
        EXPECT_FALSE(combo.matchesItem(""_atom));
        combo.setSearchText("");
        EXPECT_TRUE(combo.matchesItem("GameScene"_atom));
    }

    TEST(SearchableComboBox, ComposesSearchWithCustomFilterAndPreservesSelection)
    {
        TestSearchableComboBox combo;
        int data = 42;
        combo.setSizeProvider([] { return 1; });
        combo.setDataProvider(
            [&data](std::size_t, Core::StringAtom& label) -> const void*
            {
                label = "MenuScene"_atom;
                return &data;
            });
        combo.setCurrentIndex(0);
        combo.setItemFilter([](const Core::StringAtom& label)
                            { return label != "HiddenScene"_atom; });
        combo.setSearchText("scene");
        EXPECT_TRUE(combo.matchesItem("MenuScene"_atom));
        EXPECT_FALSE(combo.matchesItem("HiddenScene"_atom));
        combo.setSearchText("missing");
        EXPECT_FALSE(combo.matchesItem("MenuScene"_atom));
        EXPECT_EQ(combo.getCurrentIndex(), 0);
        EXPECT_EQ(combo.getCurrentData(), &data);
    }
} // namespace
