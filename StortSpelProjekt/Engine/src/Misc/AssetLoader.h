#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include "Core.h"
#include "../AudioEngine/AudioBuffer.h"
#include "assimp/matrix4x4.h"
class DescriptorHeap;
class Model;
class Mesh;
class Shader;
class Texture;
class Window;
struct Font;
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
struct aiNodeAnim;
struct aiBone;
struct Animation;
struct VertexWeight;
struct BoneInfo;
struct SkeletonNode;

class AssetLoader
{
public:
    ~AssetLoader();

	static AssetLoader* Get(ID3D12Device5* device = nullptr, DescriptorHeap* descriptorHeap_CBV_UAV_SRV = nullptr, const Window* window = nullptr);

    /* Load Functions */
    // Model ---------------
    Model* LoadModel(const std::wstring path);

    // Texture ------------
    Texture* LoadTexture(const std::wstring path);

    // Load Audio
    AudioBuffer* LoadAudio(const std::wstring& path, const std::wstring& name);
    AudioBuffer* GetAudio(const std::wstring& name);
    // ??

	// Fonts -------------
	std::pair<Font*, Texture*> LoadFontFromFile(const std::wstring fontName);

private:
    // PipelineState loads all shaders
    friend class PipelineState;
    // Renderer needs access to m_LoadedModels & m_LoadedTextures so it can check if they are uploaded to GPU.
    friend class Renderer;

    // Constructor currently called from m_pRenderer to set dx12 specific objects
    AssetLoader(ID3D12Device5* device = nullptr, DescriptorHeap* descriptorHeap_CBV_UAV_SRV = nullptr, const Window* window = nullptr);
    AssetLoader(AssetLoader const&) = delete;
    void operator=(AssetLoader const&) = delete;

    ID3D12Device5* m_pDevice = nullptr;
    DescriptorHeap* m_pDescriptorHeap_CBV_UAV_SRV = nullptr;
	Window* m_pWindow = nullptr;

    const std::wstring m_FilePathShaders = L"../Engine/src/Renderer/HLSL/";
    const std::wstring m_FilePathDefaultTextures = L"../Vendor/Resources/Textures/Default/";
	const std::wstring m_FilePathFonts = L"../Vendor/Resources/Fonts/";

    // Every model & texture also has a bool which indicates if its data is on the GPU or not
    std::map<std::wstring, std::pair<bool, Model*>> m_LoadedModels;
    std::vector<Mesh*> m_LoadedMeshes;
    std::vector<Animation*> m_LoadedAnimations;
    std::map<std::wstring, std::pair<bool, Texture*>> m_LoadedTextures;
    std::map<std::wstring, Shader*> m_LoadedShaders;
	std::map<std::wstring, std::pair<Font*, Texture*>> m_LoadedFonts;
    std::map<std::wstring, AudioBuffer> m_LoadedAudios;

    // Audio
    // add map for audio (path, AudioObject)

    /* --------------- Functions --------------- */
    void processNode(aiNode* node, 
        const aiScene* assimpScene,
        std::vector<Mesh*> *meshes,
        std::vector<std::map<TEXTURE_TYPE, Texture*>>* textures,
        const std::string* filePath);

    Mesh* processMesh(aiMesh* mesh, 
        const aiScene* assimpScene,
        std::vector<Mesh*>* meshes,
        std::vector<std::map<TEXTURE_TYPE, Texture*>>* textures,
        const std::string* filePath);

    Texture* processTexture(aiMaterial* mat, TEXTURE_TYPE texture_type, const std::string* filePathWithoutTexture);
    
    SkeletonNode* processSkeleton(std::map<std::string, BoneInfo> boneCounter, aiNode* assimpNode, const aiScene* assimpScene, std::map<unsigned int, VertexWeight>* perVertexBoneData);
    void processBones(std::map<std::string, BoneInfo> boneCounter, const aiMesh* assimpMesh, std::map<unsigned int, VertexWeight>* perVertexBoneData);
    void initializeSkeleton(SkeletonNode* node, std::map<std::string, BoneInfo> boneCounter, Animation* animation);

    void processAnimations(const aiScene* assimpScene, std::vector<Animation*>* animations);

    DirectX::XMFLOAT4X4 aiMatrix4x4ToXMFloat4x4(aiMatrix4x4* aiMatrix);
    
    Shader* loadShader(std::wstring fileName, ShaderType type);
	Font* loadFont(LPCWSTR filename, int windowWidth, int windowHeight);
};

#endif