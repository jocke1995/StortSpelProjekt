#include "stdafx.h"
#include "AssetLoader.h"

#include "../Renderer/DescriptorHeap.h"
#include "Window.h"

#include "../Renderer/Model.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Texture/Texture.h"
#include "../Renderer/Text.h"
#include "../Renderer/Animation.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "../Renderer/Texture/Texture2D.h"
#include "../Renderer/Texture/TextureCubeMap.h"

#include <DirectXMath.h>

AssetLoader::AssetLoader(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV, const Window* window)
{
	m_pDevice = device;
	m_pDescriptorHeap_CBV_UAV_SRV = descriptorHeap_CBV_UAV_SRV;
	m_pWindow = const_cast<Window*>(window);

	// Load default textures
	LoadTexture2D(m_FilePathDefaultTextures + L"default_ambient.png");
	LoadTexture2D(m_FilePathDefaultTextures + L"default_diffuse.jpg");
	LoadTexture2D(m_FilePathDefaultTextures + L"default_specular.png");
	LoadTexture2D(m_FilePathDefaultTextures + L"default_normal.png");
	LoadTexture2D(m_FilePathDefaultTextures + L"default_emissive.png");
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
	{
		delete shader.second;
	}

	// For every font
	for (auto font : m_LoadedFonts)
	{
		delete font.second.first->kerningsList;
		delete font.second.first->charList;
		delete font.second.first;
		delete font.second.second;
	}
}

AssetLoader* AssetLoader::Get(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV, const Window* window)
{
	static AssetLoader instance(device, descriptorHeap_CBV_UAV_SRV, window);

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
	std::vector<Mesh*> meshes;
	std::vector<std::map<TEXTURE2D_TYPE, Texture*>> textures;
	std::vector<Animation*> animations;

	meshes.reserve(assimpScene->mNumMeshes);
	textures.reserve(assimpScene->mNumMeshes);
	animations.reserve(assimpScene->mNumAnimations);
	m_LoadedModels[path].first = false;
	

	processNode(assimpScene->mRootNode, assimpScene, &meshes, &textures, &filePath);
	processAnimations(assimpScene, &animations);

	m_LoadedModels[path].second = new Model(path, &meshes, &animations, &textures);

	return m_LoadedModels[path].second;
}

Texture* AssetLoader::LoadTexture2D(std::wstring path)
{
	// Check if the texture already exists
	if (m_LoadedTextures.count(path) != 0)
	{
		return m_LoadedTextures[path].second;
	}

	Texture* texture = new Texture2D();
	if (texture->Init(path, m_pDevice, m_pDescriptorHeap_CBV_UAV_SRV) == false)
	{
		delete texture;
		return nullptr;
	}

	m_LoadedTextures[path].first = false;
	m_LoadedTextures[path].second = texture;
	return texture;
}

Texture* AssetLoader::LoadTextureCubeMap(std::wstring path)
{
	// Check if the texture already exists
	if (m_LoadedTextures.count(path) != 0)
	{
		return m_LoadedTextures[path].second;
	}

	Texture* texture = new TextureCubeMap();
	if (texture->Init(path, m_pDevice, m_pDescriptorHeap_CBV_UAV_SRV) == false)
	{
		delete texture;
		return nullptr;
	}

	m_LoadedTextures[path].first = false;
	m_LoadedTextures[path].second = texture;
	return texture;
}

std::pair<Font*, Texture*> AssetLoader::LoadFontFromFile(const std::wstring fontName)
{
	const std::wstring path = m_FilePathFonts + fontName;

	// Check if the font already exists
	if (m_LoadedFonts.count(path) != 0)
	{
		return m_LoadedFonts[path];
	}

	// else load the font and store it in m_LoadedFonts
	m_LoadedFonts[path].first = loadFont(path.c_str(), m_pWindow->GetScreenWidth(), m_pWindow->GetScreenHeight());

	// and create the texture
	Texture* texture = LoadTexture2D(m_LoadedFonts[path].first->fontImage);
	if (texture == nullptr)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Could not init the font texture for %s.\n", to_string(fontName).c_str());
		return m_LoadedFonts[path];
	}
	m_LoadedFonts[path].second = texture;

	return m_LoadedFonts[path];
}

AudioBuffer* AssetLoader::LoadAudio(const std::wstring& path, const std::wstring& name)
{
	if (m_LoadedAudios.count(name) != 0)
	{
		return &m_LoadedAudios[name];
	}

	m_LoadedAudios.emplace(name, path);
	return &m_LoadedAudios[name];
}

AudioBuffer* AssetLoader::GetAudio(const std::wstring& name)
{
	return &m_LoadedAudios[name];
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

void AssetLoader::processNode(aiNode* node, const aiScene* assimpScene, std::vector<Mesh*>* meshes, std::vector<std::map<TEXTURE2D_TYPE, Texture*>>* textures, const std::string* filePath)
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

Mesh* AssetLoader::processMesh(aiMesh* assimpMesh, const aiScene* assimpScene, std::vector<Mesh*>* meshes, std::vector<std::map<TEXTURE2D_TYPE, Texture*>>* textures, const std::string* filePath)
{
	// Fill this data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Bone> bones;
	std::map<TEXTURE2D_TYPE, Texture*> meshTextures;

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
		bone.offsetMatrix = aiMatrix4x4ToXMFloat4x4(&assimpBone->mOffsetMatrix);

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
	for (int i = 0; i < TEXTURE2D_TYPE::NUM_TYPES; i++)
	{
		TEXTURE2D_TYPE type = static_cast<TEXTURE2D_TYPE>(i);
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
	TEXTURE2D_TYPE texture_type,
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
	case::TEXTURE2D_TYPE::AMBIENT:
		type = aiTextureType_AMBIENT;
		defaultPath = m_FilePathDefaultTextures + L"default_ambient.png";
		warningMessageTextureType = "Ambient";
		break;
	case::TEXTURE2D_TYPE::DIFFUSE:
		type = aiTextureType_DIFFUSE;
		defaultPath = m_FilePathDefaultTextures + L"default_diffuse.jpg";
		warningMessageTextureType = "Diffuse";
		break;
	case::TEXTURE2D_TYPE::SPECULAR:
		type = aiTextureType_SPECULAR;
		defaultPath = m_FilePathDefaultTextures + L"default_specular.png";
		warningMessageTextureType = "Specular";
		break;
	case::TEXTURE2D_TYPE::NORMAL:
		type = aiTextureType_NORMALS;
		defaultPath = m_FilePathDefaultTextures + L"default_normal.png";
		warningMessageTextureType = "Normal";
		break;
	case::TEXTURE2D_TYPE::EMISSIVE:
		type = aiTextureType_EMISSIVE;
		defaultPath = m_FilePathDefaultTextures + L"default_emissive.png";
		warningMessageTextureType = "Emissive";
		break;
	}

	mat->GetTexture(type, 0, &str);
	std::string textureFile = str.C_Str();
	if (textureFile.size() != 0)
	{
		texture = LoadTexture2D(to_wstring(*filePathWithoutTexture + textureFile).c_str());
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

Font* AssetLoader::loadFont(LPCWSTR filename, int windowWidth, int windowHeight)
{
	std::wifstream fs;
	fs.open(filename);

	m_LoadedFonts[filename].first = new Font();
	std::wstring tmp;
	int startpos;

	// extract font name
	fs >> tmp >> tmp; // info face = fontname
	startpos = tmp.find(L"\"") + 1;
	m_LoadedFonts[filename].first->name = tmp.substr(startpos, tmp.size() - startpos - 1);

	// get font size
	fs >> tmp;
	startpos = tmp.find(L"=") + 1;
	m_LoadedFonts[filename].first->size = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing
	fs >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp;

	// get padding
	fs >> tmp;
	startpos = tmp.find(L"=") + 1;
	tmp = tmp.substr(startpos, tmp.size() - startpos); 

	// get up padding
	startpos = tmp.find(L",") + 1;
	m_LoadedFonts[filename].first->toppadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	// get right padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	startpos = tmp.find(L",") + 1;
	m_LoadedFonts[filename].first->rightpadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	// get down padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	startpos = tmp.find(L",") + 1;
	m_LoadedFonts[filename].first->bottompadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	// get left padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	m_LoadedFonts[filename].first->leftpadding = std::stoi(tmp) / (float)windowWidth;

	fs >> tmp;

	// get lineheight (how much to move down for each line), and normalize (between 0.0 and 1.0 based on size of font)
	fs >> tmp >> tmp;
	startpos = tmp.find(L"=") + 1;
	m_LoadedFonts[filename].first->lineHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

	// get base height (height of all characters), and normalize (between 0.0 and 1.0 based on size of font)
	fs >> tmp;
	startpos = tmp.find(L"=") + 1;
	m_LoadedFonts[filename].first->baseHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

	// get texture width
	fs >> tmp;
	startpos = tmp.find(L"=") + 1;
	m_LoadedFonts[filename].first->textureWidth = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// get texture height
	fs >> tmp;
	startpos = tmp.find(L"=") + 1;
	m_LoadedFonts[filename].first->textureHeight = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// get pages, packed, page id
	fs >> tmp >> tmp; 
	fs >> tmp >> tmp;

	// get texture filename
	std::wstring wtmp;
	fs >> wtmp;
	startpos = wtmp.find(L"\"") + 1;
	m_LoadedFonts[filename].first->fontImage = wtmp.substr(startpos, wtmp.size() - startpos - 1);
	m_LoadedFonts[filename].first->fontImage = L"../Vendor/Resources/Fonts/" + m_LoadedFonts[filename].first->fontImage;

	// get number of characters
	fs >> tmp >> tmp;
	startpos = tmp.find(L"=") + 1;
	m_LoadedFonts[filename].first->numCharacters = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// initialize the character list
	m_LoadedFonts[filename].first->charList = new FontChar[m_LoadedFonts[filename].first->numCharacters];

	for (int c = 0; c < m_LoadedFonts[filename].first->numCharacters; ++c)
	{
		// get unicode id
		fs >> tmp >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].first->charList[c].id = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get x
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].first->charList[c].u 
			= (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)m_LoadedFonts[filename].first->textureWidth;

		// get y
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].first->charList[c].v 
			= (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)m_LoadedFonts[filename].first->textureHeight;

		// get width
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		m_LoadedFonts[filename].first->charList[c].width = (float)std::stoi(tmp) / (float)windowWidth;
		m_LoadedFonts[filename].first->charList[c].twidth = (float)std::stoi(tmp) / (float)m_LoadedFonts[filename].first->textureWidth;

		// get height
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		m_LoadedFonts[filename].first->charList[c].height = (float)std::stoi(tmp) / (float)windowHeight;
		m_LoadedFonts[filename].first->charList[c].theight = (float)std::stoi(tmp) / (float)m_LoadedFonts[filename].first->textureHeight;

		// get xoffset
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].first->charList[c].xoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

		// get yoffset
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].first->charList[c].yoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

		// get xadvance
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].first->charList[c].xadvance = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

		// get page
		// get channel
		fs >> tmp >> tmp;
	}

	// get number of kernings
	fs >> tmp >> tmp;
	startpos = tmp.find(L"=") + 1;
	m_LoadedFonts[filename].first->numKernings = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// initialize the kernings list
	m_LoadedFonts[filename].first->kerningsList = new FontKerning[m_LoadedFonts[filename].first->numKernings];

	for (int k = 0; k < m_LoadedFonts[filename].first->numKernings; ++k)
	{
		// get first character
		fs >> tmp >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].first->kerningsList[k].firstid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get second character
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].first->kerningsList[k].secondid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get amount
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		int t = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos));
		m_LoadedFonts[filename].first->kerningsList[k].amount = (float)t / (float)windowWidth;
	}
	Font* font = m_LoadedFonts[filename].first;
	return m_LoadedFonts[filename].first;
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

DirectX::XMFLOAT4X4 AssetLoader::aiMatrix4x4ToXMFloat4x4(aiMatrix4x4* aiMatrix)
{
	DirectX::XMFLOAT4X4 matrix;

	matrix._11 = aiMatrix->a1;
	matrix._12 = aiMatrix->a2;
	matrix._13 = aiMatrix->a3;
	matrix._14 = aiMatrix->a4;

	matrix._21 = aiMatrix->b1;
	matrix._22 = aiMatrix->b2;
	matrix._23 = aiMatrix->b3;
	matrix._24 = aiMatrix->b4;

	matrix._31 = aiMatrix->c1;
	matrix._32 = aiMatrix->c2;
	matrix._33 = aiMatrix->c3;
	matrix._34 = aiMatrix->c4;

	matrix._41 = aiMatrix->d1;
	matrix._42 = aiMatrix->d2;
	matrix._43 = aiMatrix->d3;
	matrix._44 = aiMatrix->d4;

	return matrix;
}
