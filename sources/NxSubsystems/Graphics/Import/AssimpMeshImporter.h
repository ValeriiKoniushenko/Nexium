/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 */

#pragma once

#include "NxSubsystems/Graphics/GraphicsComponents.h"

struct aiMesh;

namespace NX
{
    class AssimpMeshImporter final
    {
    public:
        static void upload(InterleavedGraphicsData& destination, const aiMesh& mesh,
                           bool appendNormals = false, bool appendUV = false, float scale = 1.f);
    };
} // namespace NX
