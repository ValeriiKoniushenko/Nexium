/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 */

#include "AssimpMeshImporter.h"

#include "assimp/mesh.h"

namespace NX
{
    void AssimpMeshImporter::upload(InterleavedGraphicsData& destination, const aiMesh& mesh,
                                    bool appendNormals, bool appendUV, float scale)
    {
        std::vector<float> vertices;
        std::vector<GLuint> indices;

        for (unsigned int i = 0; i < mesh.mNumFaces; ++i)
        {
            const aiFace& face = mesh.mFaces[i];
            indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
        }

        for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
        {
            const aiVector3D position = mesh.mVertices[i] * scale;
            vertices.insert(vertices.end(), { position.x, position.y, position.z });

            if (appendNormals)
            {
                const aiVector3D normal = mesh.HasNormals() ? mesh.mNormals[i] : aiVector3D{};
                vertices.insert(vertices.end(), { normal.x, normal.y, normal.z });
            }

            if (appendUV)
            {
                const aiVector3D uv = mesh.HasTextureCoords(0) ? mesh.mTextureCoords[0][i]
                                                                : aiVector3D{};
                vertices.insert(vertices.end(), { uv.x, uv.y });
            }
        }

        destination.setVertexBuffer(vertices);
        destination.setIndexBuffer(indices);
    }
} // namespace NX
