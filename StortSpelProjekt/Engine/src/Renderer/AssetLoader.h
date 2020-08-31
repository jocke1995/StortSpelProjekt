#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include "DescriptorHeap.h"

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

class AssetLoader
{
public:
    ~AssetLoader();

    static AssetLoader* Get(ID3D12Device5* device = nullptr, DescriptorHeap* descriptorHeap_CBV_UAV_SRV = nullptr);

    /* Load Functions */
    // Model ---------------
    std::vector<Mesh*>* LoadModel(const std::wstring path, bool* loadedBefore);

    // Texture ------------
    Texture* LoadTexture(std::wstring path);

private:
    friend class PipelineState;
    AssetLoader(ID3D12Device5* device = nullptr, DescriptorHeap* descriptorHeap_CBV_UAV_SRV = nullptr);
    AssetLoader(AssetLoader const&) = delete;
    void operator=(AssetLoader const&) = delete;

    ID3D12Device5* device = nullptr;
    DescriptorHeap* descriptorHeap_CBV_UAV_SRV = nullptr;

    const std::wstring filePathShaders = L"../Engine/src/Renderer/HLSL/";
    const std::wstring filePathDefaultTextures = L"../Vendor/Resources/Textures/Default/";

    // Every model & texture also has a bool which indicates if its data is on the GPU or not
    std::map<std::wstring, std::pair<bool, std::vector<Mesh*>*>> loadedModels;
    std::map<std::wstring, std::pair<bool, Texture*>> loadedTextures;
    std::map<std::wstring, Shader*> loadedShaders;

    /* --------------- Functions --------------- */
    void ProcessNode(aiNode* node, const aiScene* assimpScene, std::vector<Mesh*> *meshes, const std::string* filePath);
    Mesh* ProcessMesh(aiMesh* mesh, const aiScene* assimpScene, const std::string* filePath);
    Texture* ProcessTexture(aiMaterial* mat, TEXTURE_TYPE texture_type, const std::string* filePathWithoutTexture);
    Shader* LoadShader(std::wstring fileName, ShaderType type);
};

#endif