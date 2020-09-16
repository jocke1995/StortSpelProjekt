#include "stdafx.h"
#include "AssetLoader.h"

#include "../Renderer/DescriptorHeap.h"

#include "../Renderer/Model.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Animation.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

AssetLoader::AssetLoader(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	m_pDevice = device;
	m_pDescriptorHeap_CBV_UAV_SRV = descriptorHeap_CBV_UAV_SRV;

	// Load default textures
	LoadTexture(m_FilePathDefaultTextures + L"default_ambient.png");
	LoadTexture(m_FilePathDefaultTextures + L"default_diffuse.jpg");
	LoadTexture(m_FilePathDefaultTextures + L"default_specular.png");
	LoadTexture(m_FilePathDefaultTextures + L"default_normal.png");
	LoadTexture(m_FilePathDefaultTextures + L"default_emissive.png");
}

AssetLoader::~AssetLoader()
{
	// For every Mesh
	for (auto mesh : m_LoadedMeshes)
	{
		delete mesh;
	}

	// For every Animation
	for (auto animation : m_LoadedAnimations)
	{
		delete animation;
	}

	// For every model
	for (auto pair : m_LoadedModels)
	{
		delete pair.second.second;
	}

	// For every texture
	for (auto pair : m_LoadedTextures)
	{
		delete pair.second.second;
	}

	// For every shader
	for (auto shader : m_LoadedShaders)
		delete shader.second;
}

AssetLoader* AssetLoader::Get(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	static AssetLoader instance(device, descriptorHeap_CBV_UAV_SRV);

	return &instance;
}

Model* AssetLoader::LoadModel(const std::wstring path)
{
	// Check if the model already exists
	if (m_LoadedModels.count(path) != 0)
	{
		return m_LoadedModels[path].second;
	}

	// Else load the model
	const std::string filePath(path.begin(), path.end());
	Assimp::Importer importer;

	const aiScene* assimpScene = importer.ReadFile(filePath, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded);

	if (assimpScene == nullptr)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to load model with path: \'%s\'\n", filePath.c_str());
		return nullptr;
	}

	std::vector<Mesh*>* meshes = new std::vector<Mesh*>();
	std::vector<Animation*> animations;
	std::vector<std::map<TEXTURE_TYPE, Texture*>>* textures = new std::vector<std::map<TEXTURE_TYPE, Texture*>>();

	meshes->reserve(assimpScene->mNumMeshes);
	textures->reserve(assimpScene->mNumMeshes);
	animations.reserve(assimpScene->mNumAnimations);
	m_LoadedModels[path].first = false;
	

	processNode(assimpScene->mRootNode, assimpScene, meshes, textures, &filePath);
	processAnimations(assimpScene, &animations);
	m_LoadedModels[path].second = new Model(path, meshes, &animations, textures);

	delete meshes;
	delete textures;

	return m_LoadedModels[path].second;
}

Texture* AssetLoader::LoadTexture(std::wstring path)
{
	// Check if the texture already exists
	if (m_LoadedTextures.count(path) != 0)
	{
		return m_LoadedTextures[path].second;
	}

	Texture* texture = new Texture();
	if (texture->Init(path, m_pDevice, m_pDescriptorHeap_CBV_UAV_SRV) == false)
	{
		delete texture;
		return nullptr;
	}

	m_LoadedTextures[path].first = false;
	m_LoadedTextures[path].second = texture;
	return texture;
}

Shader* AssetLoader::loadShader(std::wstring fileName, ShaderType type)
{
	// Check if the shader already exists
	if (m_LoadedShaders.count(fileName) != 0)
	{
		return m_LoadedShaders[fileName];
	}
	// else, create a new shader and compile it

	std::wstring entireFilePath = m_FilePathShaders + fileName;
	Shader* tempShader = new Shader(entireFilePath.c_str(), type);

	m_LoadedShaders[fileName] = tempShader;
	return m_LoadedShaders[fileName];
}

void AssetLoader::processNode(aiNode* node, const aiScene* assimpScene, std::vector<Mesh*>* meshes, std::vector<std::map<TEXTURE_TYPE, Texture*>>* textures, const std::string* filePath)
{
	// Go through all the m_Meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = assimpScene->mMeshes[node->mMeshes[i]];
		meshes->push_back(processMesh(mesh, assimpScene, meshes, textures, filePath));
	}
	
	// If the node has more node children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], assimpScene, meshes, textures, filePath);
	}
}

Mesh* AssetLoader::processMesh(aiMesh* assimpMesh, const aiScene* assimpScene, std::vector<Mesh*>* meshes, std::vector<std::map<TEXTURE_TYPE, Texture*>>* textures, const std::string* filePath)
{
	// Fill this data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Bone> bones;
	std::map<TEXTURE_TYPE, Texture*> meshTextures;

	// Get data from assimpMesh and store it
	for (unsigned int i = 0; i < assimpMesh->mNumVertices; i++)
	{
		Vertex vTemp = {};

		// Get positions
		if (assimpMesh->HasPositions())
		{
			vTemp.pos.x = assimpMesh->mVertices[i].x;
			vTemp.pos.y = assimpMesh->mVertices[i].y;
			vTemp.pos.z = assimpMesh->mVertices[i].z;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Mesh has no positions\n");
		}

		// Get Normals
		if (assimpMesh->HasNormals())
		{
			vTemp.normal.x = assimpMesh->mNormals[i].x;
			vTemp.normal.y = assimpMesh->mNormals[i].y;
			vTemp.normal.z = assimpMesh->mNormals[i].z;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Mesh has no normals\n");
		}

		if (assimpMesh->HasTangentsAndBitangents())
		{
			vTemp.tangent.x = assimpMesh->mTangents[i].x;
			vTemp.tangent.y = assimpMesh->mTangents[i].y;
			vTemp.tangent.z = assimpMesh->mTangents[i].z;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Mesh has no tangents\n");
		}
		
		
		// Get texture coordinates if there are any
		if (assimpMesh->HasTextureCoords(0))
		{
			vTemp.uv.x = (float)assimpMesh->mTextureCoords[0][i].x;
			vTemp.uv.y = (float)assimpMesh->mTextureCoords[0][i].y;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Mesh has no textureCoords\n");
		}

		vertices.push_back(vTemp);
	}

	// Get indices
	for (unsigned int i = 0; i < assimpMesh->mNumFaces; i++)
	{
		aiFace face = assimpMesh->mFaces[i];
	
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Get bones
	for (unsigned int i = 0; i < assimpMesh->mNumBones; i++)
	{
		Bone bone;
		aiBone* assimpBone = assimpMesh->mBones[i];

		// Store the name of the bone
		bone.name = assimpBone->mName.C_Str();	// Possible loss of data, returns pointer.

		// Store the offset matrix of the bone
		DirectX::XMFLOAT4X4 offsetMat(&assimpBone->mOffsetMatrix.a1);	// Possible loss of data, might be the wrong pointer to data
		DirectX::XMStoreFloat4x4(&offsetMat, bone.offsetMatrix);

		// Store the weights of the bone
		for (unsigned int j = 0; j < assimpBone->mNumWeights; j++)
		{
			VertexWeight weight;
			weight.vertexID = assimpBone->mWeights[j].mVertexId;
			weight.weight = assimpBone->mWeights[j].mWeight;
			bone.weights.push_back(weight);
		}

		// Store the bone in the mesh
		bones.push_back(bone);
	}


	// Create Mesh
	Mesh* mesh = new Mesh(
		m_pDevice,
		&vertices, &indices, &bones,
		m_pDescriptorHeap_CBV_UAV_SRV,
		*filePath);

	// save mesh
	m_LoadedMeshes.push_back(mesh);

	// ---------- Get Textures and set them to the m_pMesh START----------
	aiMaterial* mat = assimpScene->mMaterials[assimpMesh->mMaterialIndex];
	
	// Split filepath
	std::string filePathWithoutTexture = *filePath;
	std::size_t indicesInPath = filePathWithoutTexture.find_last_of("/\\");
	filePathWithoutTexture = filePathWithoutTexture.substr(0, indicesInPath + 1);

	// Add the textures to the m_pMesh
	for (int i = 0; i < TEXTURE_TYPE::NUM_TEXTURE_TYPES; i++)
	{
		TEXTURE_TYPE type = static_cast<TEXTURE_TYPE>(i);
		Texture* texture = processTexture(mat, type, &filePathWithoutTexture);
		meshTextures[type] = texture;
	}
	// add the texture to the correct mesh (later for models slotinfo)
	textures->push_back(meshTextures);
	// ---------- Get Textures and set them to the m_pMesh END----------

	// Set shininess
	float shininess = 100;
	// Todo: looks to bright with these values, bad models or bad scene?
	// if (AI_SUCCESS != aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shininess))
	// {
	// 	// if unsuccessful set a default
	// 	shininess = 20.0f;
	// }

	return mesh;
}

Texture* AssetLoader::processTexture(aiMaterial* mat,
	TEXTURE_TYPE texture_type,
	const std::string* filePathWithoutTexture)
{
	aiTextureType type;
	aiString str;
	Texture* texture = nullptr;

	// incase of the texture doesn't exist
	std::wstring defaultPath = L"";
	std::string warningMessageTextureType = "";

	// Find the textureType
	switch (texture_type)
	{
	case::TEXTURE_TYPE::AMBIENT:
		type = aiTextureType_AMBIENT;
		defaultPath = m_FilePathDefaultTextures + L"default_ambient.png";
		warningMessageTextureType = "Ambient";
		break;
	case::TEXTURE_TYPE::DIFFUSE:
		type = aiTextureType_DIFFUSE;
		defaultPath = m_FilePathDefaultTextures + L"default_diffuse.jpg";
		warningMessageTextureType = "Diffuse";
		break;
	case::TEXTURE_TYPE::SPECULAR:
		type = aiTextureType_SPECULAR;
		defaultPath = m_FilePathDefaultTextures + L"default_specular.png";
		warningMessageTextureType = "Specular";
		break;
	case::TEXTURE_TYPE::NORMAL:
		type = aiTextureType_NORMALS;
		defaultPath = m_FilePathDefaultTextures + L"default_normal.png";
		warningMessageTextureType = "Normal";
		break;
	case::TEXTURE_TYPE::EMISSIVE:
		type = aiTextureType_EMISSIVE;
		defaultPath = m_FilePathDefaultTextures + L"default_emissive.png";
		warningMessageTextureType = "Emissive";
		break;
	}

	mat->GetTexture(type, 0, &str);
	std::string textureFile = str.C_Str();
	if (textureFile.size() != 0)
	{
		texture = LoadTexture(to_wstring(*filePathWithoutTexture + textureFile).c_str());
	}

	if (texture != nullptr)
	{
		return texture;
	}
	else
	{
		// No texture, warn and apply default Texture
		Log::PrintSeverity(Log::Severity::WARNING, "Applying default texture: " + warningMessageTextureType + 
			" on mesh with path: \'%s\'\n", filePathWithoutTexture->c_str());
		return m_LoadedTextures[defaultPath].second;
	}

	return nullptr;
}

void AssetLoader::processAnimations(const aiScene* assimpScene, std::vector<Animation*>* animations)
{
	// Store the animations
	for (unsigned int i = 0; i < assimpScene->mNumAnimations; i++)
	{
		Animation* animation = new Animation();
		aiAnimation* assimpAnimation = assimpScene->mAnimations[i];

		animation->duration = assimpAnimation->mDuration;
		animation->ticksPerSecond = assimpAnimation->mTicksPerSecond;

		// Store the transform data for each nodeAnimation
		for (unsigned int j = 0; j < assimpAnimation->mNumChannels; j++)
		{
			NodeAnimation nodeAnimation;
			aiNodeAnim* assimpNodeAnimation = assimpAnimation->mChannels[j];
			processNodeAnimation(assimpAnimation->mChannels[j], &nodeAnimation);
			animation->nodeAnimations.push_back(nodeAnimation);
		}

		// Save the pointer both in the model and the asset loader
		animations->push_back(animation);
		m_LoadedAnimations.push_back(animation);
	}
}

void AssetLoader::processNodeAnimation(const aiNodeAnim* assimpNodeAnimation, NodeAnimation* nodeAnimation)
{
	// possibly do a new here for the nodeanimation
	// Store the name. This name will be used to know which node (bone) this nodeAnimation belongs to.
	nodeAnimation->name = assimpNodeAnimation->mNodeName.C_Str();

	// Store the positions
	for (unsigned int i = 0; i < assimpNodeAnimation->mNumPositionKeys; i++)
	{
		nodeAnimation->positions.push_back(
			DirectX::XMFLOAT3(
				assimpNodeAnimation->mPositionKeys[i].mValue.x,
				assimpNodeAnimation->mPositionKeys[i].mValue.y,
				assimpNodeAnimation->mPositionKeys[i].mValue.z));
	}

	// Store the rotation quaternions
	for (unsigned int i = 0; i < assimpNodeAnimation->mNumRotationKeys; i++)
	{
		nodeAnimation->rotationQuaternions.push_back(
			DirectX::XMFLOAT4(
				assimpNodeAnimation->mRotationKeys[i].mValue.x,
				assimpNodeAnimation->mRotationKeys[i].mValue.y,
				assimpNodeAnimation->mRotationKeys[i].mValue.z,
				assimpNodeAnimation->mRotationKeys[i].mValue.w));
	}

	// Store the scale values
	for (unsigned int i = 0; i < assimpNodeAnimation->mNumScalingKeys; i++)
	{
		nodeAnimation->scalings.push_back(
			DirectX::XMFLOAT3(
				assimpNodeAnimation->mScalingKeys[i].mValue.x,
				assimpNodeAnimation->mScalingKeys[i].mValue.y,
				assimpNodeAnimation->mScalingKeys[i].mValue.z));
	}
}
