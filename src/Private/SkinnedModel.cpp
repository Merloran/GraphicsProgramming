#include "Public/SkinnedModel.h"
#include "Public/Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <iostream>

SkinnedModel::SkinnedModel(const char* Path)
{
    LoadModel(Path);
}

void SkinnedModel::Draw(Shader& Shader)
{
    Shader.Use();

    for (SkinnedMesh& mesh : m_Meshes)
    {
        mesh.Draw(Shader);
    }
}

SkinnedMesh& SkinnedModel::GetMesh(uint32_t Index)
{
    if (Index > m_Meshes.size())
    {
        std::cout << "Index out of range returned last element." << std::endl;
        return m_Meshes[m_Meshes.size() - 1];
    }
    return m_Meshes[Index];
}

uint32_t SkinnedModel::GetMeshCount() const
{
    return m_Meshes.size();
}

void SkinnedModel::SetVertexBoneDataToDefault(SkinnedVertex& vertex)
{
    for (int32_t i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        vertex.BoneIDs[i] = -1;
        vertex.Weights[i] = 0.0f;
    }
}

void SkinnedModel::SetVertexBoneData(SkinnedVertex& vertex, int32_t boneID, float weight)
{
    for (int32_t i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if (vertex.BoneIDs[i] < 0)
        {
            vertex.Weights[i] = weight;
            vertex.BoneIDs[i] = boneID;
            break;
        }
    }
}

void SkinnedModel::LoadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        fprintf(stdout, "ERROR::ASSIMP::%s\n", import.GetErrorString());
        return;
    }
    m_Directory = path.substr(0, path.find_last_of('/'));

    ProcessNode(scene->mRootNode, scene);
}

void SkinnedModel::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (uint32_t i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(ProcessMesh(mesh, scene));
    }
    for (uint32_t i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

SkinnedMesh SkinnedModel::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<SkinnedVertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;

    for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
    {
        SkinnedVertex vertex;
        glm::vec3 vector;
        SetVertexBoneDataToDefault(vertex);

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f);
        }
        vertices.push_back(vertex);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];


        //for (int i = 0; i < aiTextureType::aiTextureType_UNKNOWN; ++i)
        //{
        //    std::cout << aiTextureTypeToString(aiTextureType(aiTextureType_NONE + i)) << ": " << material->GetTextureCount(aiTextureType(aiTextureType_NONE + i)) << std::endl;
        //}
        //std::cout << std::endl;

        // ALBEDO MAP
        std::vector<Texture> albedoMaps;
        if (material->GetTextureCount(aiTextureType(aiTextureType_DIFFUSE)))
        {
            albedoMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::ALBEDO);
        }
        else if (material->GetTextureCount(aiTextureType(aiTextureType_BASE_COLOR)))
        {
            albedoMaps = LoadMaterialTextures(material, aiTextureType_BASE_COLOR, TextureType::ALBEDO);
        }
        textures.insert(textures.end(), albedoMaps.begin(), albedoMaps.end());

        // NORMAL MAP
        std::vector<Texture> normalMaps;
        if (material->GetTextureCount(aiTextureType(aiTextureType_NORMALS)))
        {
            normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, TextureType::NORMAL);
        }
        else if (material->GetTextureCount(aiTextureType(aiTextureType_HEIGHT)))
        {
            normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, TextureType::NORMAL);
        }
        else if (material->GetTextureCount(aiTextureType(aiTextureType_NORMAL_CAMERA)))
        {
            normalMaps = LoadMaterialTextures(material, aiTextureType_NORMAL_CAMERA, TextureType::NORMAL);
        }
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // EMISSION MAP
        std::vector<Texture> emissionMaps;
        if (material->GetTextureCount(aiTextureType(aiTextureType_EMISSIVE)))
        {
            emissionMaps = LoadMaterialTextures(material, aiTextureType_EMISSIVE, TextureType::EMISSION);
        }
        else if (material->GetTextureCount(aiTextureType(aiTextureType_EMISSION_COLOR)))
        {
            emissionMaps = LoadMaterialTextures(material, aiTextureType_EMISSION_COLOR, TextureType::EMISSION);
        }
        textures.insert(textures.end(), emissionMaps.begin(), emissionMaps.end());

        // METALNESS MAP
        std::vector<Texture> metalnessMaps;
        if (material->GetTextureCount(aiTextureType(aiTextureType_SPECULAR)))
        {
            metalnessMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::METALNESS);
        }
        //else if (material->GetTextureCount(aiTextureType(aiTextureType_METALNESS)))
        //{
        //    metalnessMaps = LoadMaterialTextures(material, aiTextureType_METALNESS, TextureType::METALNESS);
        //}
        textures.insert(textures.end(), metalnessMaps.begin(), metalnessMaps.end());

        // ROUGHNESS MAP
        std::vector<Texture> roughnessMaps;
        if (material->GetTextureCount(aiTextureType(aiTextureType_SHININESS)))
        {
            roughnessMaps = LoadMaterialTextures(material, aiTextureType_SHININESS, TextureType::ROUGHNESS);
        }
        //else if (material->GetTextureCount(aiTextureType(aiTextureType_DIFFUSE_ROUGHNESS)))
        //{
        //    roughnessMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, TextureType::ROUGHNESS);
        //}
        textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

        // AMBIENT OCCLUSION MAP
        std::vector<Texture> ambientOcclusionMaps;
        if (material->GetTextureCount(aiTextureType(aiTextureType_LIGHTMAP)))
        {
            ambientOcclusionMaps = LoadMaterialTextures(material, aiTextureType_LIGHTMAP, TextureType::AMBIENTOCCLUSION);
        }
        else if (material->GetTextureCount(aiTextureType(aiTextureType_AMBIENT)))
        {
            ambientOcclusionMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, TextureType::AMBIENTOCCLUSION);
        }
        else if (material->GetTextureCount(aiTextureType(aiTextureType_AMBIENT_OCCLUSION)))
        {
            ambientOcclusionMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, TextureType::AMBIENTOCCLUSION);
        }
        textures.insert(textures.end(), ambientOcclusionMaps.begin(), ambientOcclusionMaps.end());
    }

    ExtractBoneWeightForVertices(vertices, mesh, scene);

    return SkinnedMesh(vertices, indices, textures);
}

void SkinnedModel::ExtractBoneWeightForVertices(std::vector<SkinnedVertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    for (int32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int32_t boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
        {
            aiVector3D pos;
            aiQuaternion rot;
            BoneInfo newBoneInfo;
            newBoneInfo.ID = m_BoneCounter;
            //newBoneInfo.Offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            mesh->mBones[boneIndex]->mOffsetMatrix.DecomposeNoScaling(rot, pos);
            newBoneInfo.Position.x = pos.x;
            newBoneInfo.Position.y = pos.y;
            newBoneInfo.Position.z = pos.z;

            newBoneInfo.Rotation.x = rot.x;
            newBoneInfo.Rotation.y = rot.y;
            newBoneInfo.Rotation.z = rot.z;
            newBoneInfo.Rotation.w = rot.w;

            m_BoneInfoMap[boneName] = newBoneInfo;
            boneID = m_BoneCounter;
            m_BoneCounter++;
        }
        else
        {
            boneID = m_BoneInfoMap[boneName].ID;
        }
        assert(boneID != -1);
        aiVertexWeight* weights = mesh->mBones[boneIndex]->mWeights;
        uint32_t numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (uint32_t weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            uint32_t vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

std::vector<Texture> SkinnedModel::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        for (unsigned int j = 0; j < m_TexturesLoaded.size(); ++j)
        {
            // if texture.path and str are equals
            if (std::strcmp(m_TexturesLoaded[j].GetPath().data(), (m_Directory + std::string(str.C_Str())).c_str()) == 0)
            {
                textures.push_back(m_TexturesLoaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            mat->GetTexture(type, i, &str);
            Texture texture;
            if (typeName == TextureType::ALBEDO)
            {
                texture = Texture(typeName, m_Directory + "/" + str.C_Str()); // Sometimes albedo texture is in SRGB(A) then set 2nd paramater as true
            }
            else
            {
                texture = Texture(typeName, m_Directory + "/" + str.C_Str());
            }
            textures.push_back(texture);
        }
    }
    return textures;
}