#include "Public/Model.h"
#include "Public/Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <iostream>

Model::Model(const char* Path)
{
    LoadModel(Path);
}

void Model::Draw(Shader& Shader)
{
    Shader.Use();

    for (Mesh& mesh : m_Meshes)
    {
        mesh.Draw(Shader);
    }
}

Mesh& Model::GetMesh(unsigned int Index)
{
    if (Index > m_Meshes.size())
    {
        std::cout << "Index out of range returned last element." << std::endl;
        return m_Meshes[m_Meshes.size() - 1];
    }
    return m_Meshes[Index];
}

unsigned int Model::GetMeshCount() const
{
    return m_Meshes.size();
}

void Model::LoadModel(std::string path)
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

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(ProcessMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        glm::vec3 vector;

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

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
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

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType typeName)
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
                texture = Texture(typeName, m_Directory + "/" + str.C_Str(), true);
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