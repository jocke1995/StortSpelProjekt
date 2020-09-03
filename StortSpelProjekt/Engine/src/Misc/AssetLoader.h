#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include "Core.h"

class DescriptorHeap;
class Mesh;
class Shader;
class Texture;
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;


class AssetLoader
{
public:
    ~AssetLoader();

    static AssetLoader* Get(ID3D12Device5* device = nullptr, DescriptorHeap* descriptorHeap_CBV_UAV_SRV = nullptr);

    /* Load Functions */
    // Model ---------------
    std::vector<Mesh*>* LoadModel(const std::wstring path);

    // Texture ------------
    Texture* LoadTexture(std::wstring path);

private:
    // PipelineState loads all shaders
    friend class PipelineState;
    // Renderer needs access to m_LoadedModels & m_LoadedTextures so it can check if they are uploaded to GPU.
    friend class Renderer;

    // Constructor currently called from m_pRenderer to set dx12 specific objects
    AssetLoader(ID3D12Device5* device = nullptr, DescriptorHeap* descriptorHeap_CBV_UAV_SRV = nullptr);
    AssetLoader(AssetLoader const&) = delete;
    void operator=(AssetLoader const&) = delete;

    ID3D12Device5* m_pDevice = nullptr;
    DescriptorHeap* m_pDescriptorHeap_CBV_UAV_SRV = nullptr;

    const std::wstring m_FilePathShaders = L"../Engine/src/Renderer/HLSL/";
    const std::wstring m_FilePathDefaultTextures = L"../Vendor/Resources/Textures/Default/";

    // Every model & texture also has a bool which indicates if its data is on the GPU or not
    std::map<std::wstring, std::pair<bool, std::vector<Mesh*>*>> m_LoadedModels;
    std::map<std::wstring, std::pair<bool, Texture*>> m_LoadedTextures;
    std::map<std::wstring, Shader*> m_LoadedShaders;

    /* --------------- Functions --------------- */
    void processNode(aiNode* node, const aiScene* assimpScene, std::vector<Mesh*> *meshes, const std::string* filePath);
    Mesh* processMesh(aiMesh* mesh, const aiScene* assimpScene, const std::string* filePath);
    Texture* processTexture(aiMaterial* mat, TEXTURE_TYPE texture_type, const std::string* filePathWithoutTexture);
    Shader* loadShader(std::wstring fileName, ShaderType type);
};

#endif