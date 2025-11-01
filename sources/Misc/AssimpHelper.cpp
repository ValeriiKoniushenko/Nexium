/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "AssimpHelper.h"

namespace Assimp
{
    Core::StringAtom aiPostProcessStepsToString(aiPostProcessSteps val)
    {
        // clang-format off
        if (val == aiProcess_CalcTangentSpace) return "CalcTangentSpace"_atom;
        if (val == aiProcess_Debone) return "Debone"_atom;
        if (val == aiProcess_DropNormals) return "DropNormals"_atom;
        if (val == aiProcess_EmbedTextures) return "EmbedTextures"_atom;
        if (val == aiProcess_FindDegenerates) return "FindDegenerates"_atom;
        if (val == aiProcess_FindInstances) return "FindInstances"_atom;
        if (val == aiProcess_FindInvalidData) return "FindInvalidData"_atom;
        if (val == aiProcess_FixInfacingNormals) return "FixInfacingNormals"_atom;
        if (val == aiProcess_FlipUVs) return "FlipUVs"_atom;
        if (val == aiProcess_FlipWindingOrder) return "FlipWindingOrder"_atom;
        if (val == aiProcess_ForceGenNormals) return "ForceGenNormals"_atom;
        if (val == aiProcess_GenBoundingBoxes) return "GenBoundingBoxes"_atom;
        if (val == aiProcess_GenNormals) return "GenNormals"_atom;
        if (val == aiProcess_GenSmoothNormals) return "GenSmoothNormals"_atom;
        if (val == aiProcess_GenUVCoords) return "GenUVCoords"_atom;
        if (val == aiProcess_GlobalScale) return "GlobalScale"_atom;
        if (val == aiProcess_ImproveCacheLocality) return "ImproveCacheLocality"_atom;
        if (val == aiProcess_JoinIdenticalVertices) return "JoinIdenticalVertices"_atom;
        if (val == aiProcess_LimitBoneWeights) return "LimitBoneWeights"_atom;
        if (val == aiProcess_MakeLeftHanded) return "MakeLeftHanded"_atom;
        if (val == aiProcess_OptimizeGraph) return "OptimizeGraph"_atom;
        if (val == aiProcess_OptimizeMeshes) return "OptimizeMeshes"_atom;
        if (val == aiProcess_PopulateArmatureData) return "PopulateArmatureData"_atom;
        if (val == aiProcess_PreTransformVertices) return "PreTransformVertices"_atom;
        if (val == aiProcess_RemoveComponent) return "RemoveComponent"_atom;
        if (val == aiProcess_RemoveRedundantMaterials) return "RemoveRedundantMaterials"_atom;
        if (val == aiProcess_SortByPType) return "SortByPType"_atom;
        if (val == aiProcess_SplitByBoneCount) return "SplitByBoneCount"_atom;
        if (val == aiProcess_SplitLargeMeshes) return "SplitLargeMeshes"_atom;
        if (val == aiProcess_TransformUVCoords) return "TransformUVCoords"_atom;
        if (val == aiProcess_Triangulate) return "Triangulate"_atom;
        if (val == aiProcess_ValidateDataStructure) return "ValidateDataStructure"_atom;
        // clang-format on
        Assert(false);
        return Core::StringAtom(""_atom);
    }

    std::optional<aiPostProcessSteps> aiPostProcessStepsFromString(const Core::StringAtom& val)
    {
        // clang-format off
        if (val == "CalcTangentSpace"_atom) return aiProcess_CalcTangentSpace;
        if (val == "Debone"_atom) return aiProcess_Debone;
        if (val == "DropNormals"_atom) return aiProcess_DropNormals;
        if (val == "EmbedTextures"_atom) return aiProcess_EmbedTextures;
        if (val == "FindDegenerates"_atom) return aiProcess_FindDegenerates;
        if (val == "FindInstances"_atom) return aiProcess_FindInstances;
        if (val == "FindInvalidData"_atom) return aiProcess_FindInvalidData;
        if (val == "FixInfacingNormals"_atom) return aiProcess_FixInfacingNormals;
        if (val == "FlipUVs"_atom) return aiProcess_FlipUVs;
        if (val == "FlipWindingOrder"_atom) return aiProcess_FlipWindingOrder;
        if (val == "ForceGenNormals"_atom) return aiProcess_ForceGenNormals;
        if (val == "GenBoundingBoxes"_atom) return aiProcess_GenBoundingBoxes;
        if (val == "GenNormals"_atom) return aiProcess_GenNormals;
        if (val == "GenSmoothNormals"_atom) return aiProcess_GenSmoothNormals;
        if (val == "GenUVCoords"_atom) return aiProcess_GenUVCoords;
        if (val == "GlobalScale"_atom) return aiProcess_GlobalScale;
        if (val == "ImproveCacheLocality"_atom) return aiProcess_ImproveCacheLocality;
        if (val == "JoinIdenticalVertices"_atom) return aiProcess_JoinIdenticalVertices;
        if (val == "LimitBoneWeights"_atom) return aiProcess_LimitBoneWeights;
        if (val == "MakeLeftHanded"_atom) return aiProcess_MakeLeftHanded;
        if (val == "OptimizeGraph"_atom) return aiProcess_OptimizeGraph;
        if (val == "OptimizeMeshes"_atom) return aiProcess_OptimizeMeshes;
        if (val == "PopulateArmatureData"_atom) return aiProcess_PopulateArmatureData;
        if (val == "PreTransformVertices"_atom) return aiProcess_PreTransformVertices;
        if (val == "RemoveComponent"_atom) return aiProcess_RemoveComponent;
        if (val == "RemoveRedundantMaterials"_atom) return aiProcess_RemoveRedundantMaterials;
        if (val == "SortByPType"_atom) return aiProcess_SortByPType;
        if (val == "SplitByBoneCount"_atom) return aiProcess_SplitByBoneCount;
        if (val == "SplitLargeMeshes"_atom) return aiProcess_SplitLargeMeshes;
        if (val == "TransformUVCoords"_atom) return aiProcess_TransformUVCoords;
        if (val == "Triangulate"_atom) return aiProcess_Triangulate;
        if (val == "ValidateDataStructure"_atom) return aiProcess_ValidateDataStructure;
        // clang-format on

        Assert(false);
        return std::nullopt;
    }

    const std::vector<aiPostProcessSteps> aiPostProcessStepsAsVector = {
        aiProcess_CalcTangentSpace,
        aiProcess_Debone,
        aiProcess_DropNormals,
        aiProcess_EmbedTextures,
        aiProcess_FindDegenerates,
        aiProcess_FindInstances,
        aiProcess_FindInvalidData,
        aiProcess_FixInfacingNormals,
        aiProcess_FlipUVs,
        aiProcess_FlipWindingOrder,
        aiProcess_ForceGenNormals,
        aiProcess_GenBoundingBoxes,
        aiProcess_GenNormals,
        aiProcess_GenSmoothNormals,
        aiProcess_GenUVCoords,
        aiProcess_GlobalScale,
        aiProcess_ImproveCacheLocality,
        aiProcess_JoinIdenticalVertices,
        aiProcess_LimitBoneWeights,
        aiProcess_MakeLeftHanded,
        aiProcess_OptimizeGraph,
        aiProcess_OptimizeMeshes,
        aiProcess_PopulateArmatureData,
        aiProcess_PreTransformVertices,
        aiProcess_RemoveComponent,
        aiProcess_RemoveRedundantMaterials,
        aiProcess_SortByPType,
        aiProcess_SplitByBoneCount,
        aiProcess_SplitLargeMeshes,
        aiProcess_TransformUVCoords,
        aiProcess_Triangulate,
        aiProcess_ValidateDataStructure,
    };
} // namespace Assimp
