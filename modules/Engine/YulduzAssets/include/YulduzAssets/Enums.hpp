#pragma once

#include <Yulduz/Core.hpp>

namespace Yulduz {
    enum class AssimpPostProcess {
        None = 0,
        CalcTangentSpace = aiProcess_CalcTangentSpace,
        // JoinIdenticalVertices = aiProcess_JoinIdenticalVertices,
        // MakeLeftHanded = aiProcess_MakeLeftHanded,
        RemoveComponent = aiProcess_RemoveComponent,
        GenNormals = aiProcess_GenNormals,
        GenSmoothNormals = aiProcess_GenSmoothNormals,
        SplitLargeMeshes = aiProcess_SplitLargeMeshes,
        PreTransformVertices = aiProcess_PreTransformVertices,
        LimitBoneWeights = aiProcess_LimitBoneWeights,
        ValidateDataStructure = aiProcess_ValidateDataStructure,
        ImproveCacheLocality = aiProcess_ImproveCacheLocality,
        RemoveRedundantMaterials = aiProcess_RemoveRedundantMaterials,
        FixInfacingNormals = aiProcess_FixInfacingNormals,
        PopulateArmatureData = aiProcess_PopulateArmatureData,
        SortByPType = aiProcess_SortByPType,
        FindDegenerates = aiProcess_FindDegenerates,
        FindInvalidData = aiProcess_FindInvalidData,
        GenUVCoords = aiProcess_GenUVCoords,
        TransformUVCoords = aiProcess_TransformUVCoords,
        FindInstances = aiProcess_FindInstances,
        OptimizeMeshes = aiProcess_OptimizeMeshes,
        OptimizeGraph = aiProcess_OptimizeGraph,
        FlipUVs = aiProcess_FlipUVs,
        FlipWindingOrder = aiProcess_FlipWindingOrder,
        SplitByBoneCount = aiProcess_SplitByBoneCount,
        Debone = aiProcess_Debone,
        GlobalScale = aiProcess_GlobalScale,
        EmbedTextures = aiProcess_EmbedTextures,
        ForceGenNormals = aiProcess_ForceGenNormals,
        DropNormals = aiProcess_DropNormals,
        GenBoundingBoxes = aiProcess_GenBoundingBoxes,
    };

    YULDUZ_ENUM_UINT_BITWISE_OPERATORS(AssimpPostProcess)
}  // namespace Yulduz
