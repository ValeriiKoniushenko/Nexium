// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Documents an ownership hazard in the BaseGraphicsData family. Expected to FAIL until copies
// stop aliasing GPU handles (deleted copy operations, or a copy that re-creates its own
// buffers / shares them through a ref-counted handle).

#include "NxSubsystems/Graphics/GraphicsComponents.h"

#include "gtest/gtest.h"
#include <type_traits>

TEST(GraphicsOwnershipGaps, GraphicsDataDoesNotAllowCopiesThatAliasGpuHandles)
{
    // Every class in the tree owns raw GL names (VAO/VBO/EBO/texture) and deletes them in its
    // destructor, yet the copy constructor and copy assignment are defaulted. A copy therefore
    // holds the same names and glDelete* runs twice: the first destructor frees the buffers
    // the surviving copy still draws with. Today the move operations are correct, but nothing
    // stops a copy: StaticMesh inherits InterleavedGraphicsData and ECS clone() copies it, and
    // any future component that embeds graphics data by value (sprite batches, spline meshes)
    // inherits the same trap.
    EXPECT_FALSE(std::is_copy_constructible_v<NX::BaseGraphicsData>);
    EXPECT_FALSE(std::is_copy_assignable_v<NX::BaseGraphicsData>);
    EXPECT_FALSE(std::is_copy_constructible_v<NX::BaseTextureGraphicsData>);
    EXPECT_FALSE(std::is_copy_constructible_v<NX::InterleavedGraphicsData>);
    EXPECT_FALSE(std::is_copy_constructible_v<NX::SeparTextureGraphicsData>);

    // Moves remain the way to transfer ownership.
    EXPECT_TRUE(std::is_nothrow_move_constructible_v<NX::BaseGraphicsData>);
    EXPECT_TRUE(std::is_nothrow_move_assignable_v<NX::BaseGraphicsData>);
}
