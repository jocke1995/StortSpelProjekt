#include "stdafx.h"
#include "AssetLoader.h"

AssetLoader::AssetLoader(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	this->device = device;
	this->descriptorHeap_CBV_UAV_SRV = descriptorHeap_CBV_UAV_SRV;

	// Load default textures
	LoadTexture(this->filePathDefaultTextures + L"default_ambient.png");
	LoadTexture(this->filePathDefaultTextures + L"default_diffuse.jpg");
	LoadTexture(this->filePathDefaultTextures + L"default_specular.png");
	LoadTexture(this->filePathDefaultTextures + L"default_normal.png");
	LoadTexture(this->filePathDefaultTextures + L"default_emissive.png");
}

AssetLoader::~AssetLoader()
{
	// For every model
	for (auto pair : this->loadedModels)
	{
		// For every mesh the model has
		for (unsigned int i = 0; i < pair.second.second->size(); i++)
		{
			delete pair.second.second->at(i);
		}
		delete pair.second.second;
	}

	// For every texture
	for (auto pair : this->loadedTextures)
	{
		delete pair.second.second;
	}

	// For every shader
	for (auto shader : this->loadedShaders)
		delete shader.second;
}

AssetLoader* AssetLoader::Get(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	static AssetLoader instance(device, descriptorHeap_CBV_UAV_SRV);

	return &instance;
}

std::vector<Mesh*>* AssetLoader::LoadModel(const std::wstring path, bool* loadedBefore)
{
	// Check if the model already exists
	if (this->loadedModels.count(path) != 0)
	{
		*loadedBefore = true;
		return this->loadedModels[path].second;
	}

	// Else load the model
	const std::string filePath(path.begin(), path.end());
	Assimp::Importer importer;

	const aiScene* assimpScene = importer.ReadFile(filePath, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);

	if (assimpScene == nullptr)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to load model with path: \'%s\'\n", filePath.c_str());
		return nullptr;
	}
	
	std::vector<Mesh*> *meshes = new std::vector<Mesh*>;
	meshes->reserve(assimpScene->mNumMeshes);
	this->loadedModels[path].first = false;
	this->loadedModels[path].second = meshes;

	this->ProcessNode(assimpScene->mRootNode, assimpScene, meshes, &filePath);

	*loadedBefore = false;
	return this->loadedModels[path].second;
}

Texture* AssetLoader::LoadTexture(std::wstring path)
{
	// Check if the texture already exists
	if (this->loadedTextures.count(path) != 0)
	{
		return this->loadedTextures[path].second;
	}

	Texture* texture = new Texture();
	if (texture->Init(path, this->device, this->descriptorHeap_CBV_UAV_SRV) == false)
	{
		delete texture;
		return nullptr;
	}

	this->loadedTextures[path].first = false;
	this->loadedTextures[path].second = texture;
	return texture;
}

Shader* AssetLoader::LoadShader(std::wstring fileName, ShaderType type)
{
	// Check if the shader already exists
	if (loadedShaders.count(fileName) != 0)
	{
		return loadedShaders[fileName];
	}
	// else, create a new shader and compile it

	std::wstring entireFilePath = filePathShaders + fileName;
	Shader* tempShader = new Shader(entireFilePath.c_str(), type);

	loadedShaders[fileName] = tempShader;
	return loadedShaders[fileName];
}

void AssetLoader::ProcessNode(aiNode* node, const aiScene* assimpScene, std::vector<Mesh*>* meshes, const std::string* filePath)
{
	// Go through all the meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = assimpScene->mMeshes[node->mMeshes[i]];
		meshes->push_back(this->ProcessMesh(mesh, assimpScene, filePath));
	}
	
	// If the node has more node children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], assimpScene, meshes, filePath);
	}
}

Mesh* AssetLoader::ProcessMesh(aiMesh* assimpMesh, const aiScene* assimpScene, const std::string* filePath)
{
	// Fill this data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::map<TEXTURE_TYPE, Texture*> textures;

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
			vTemp.pos.w = 1.0;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Mesh has no positions");
		}

		// Get Normals
		if (assimpMesh->HasNormals())
		{
			vTemp.normal.x = assimpMesh->mNormals[i].x;
			vTemp.normal.y = assimpMesh->mNormals[i].y;
			vTemp.normal.z = assimpMesh->mNormals[i].z;
			vTemp.normal.w = 0.0;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Mesh has no normals");
		}

		if (assimpMesh->HasTangentsAndBitangents())
		{
			vTemp.tangent.x = assimpMesh->mTangents[i].x;
			vTemp.tangent.y = assimpMesh->mTangents[i].y;
			vTemp.tangent.z = assimpMesh->mTangents[i].z;
			vTemp.tangent.w = 0.0;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Mesh has no tangents");
		}
		
		
		// Get texture coordinates if there are any
		if (assimpMesh->HasTextureCoords(0))
		{
			vTemp.uv.x = (float)assimpMesh->mTextureCoords[0][i].x;
			vTemp.uv.y = (float)assimpMesh->mTextureCoords[0][i].y;
			vTemp.uv.z = 0.0f;
			vTemp.uv.w = 0.0f;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Mesh has no textureCoords");
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

	// Create Mesh
	Mesh* mesh = new Mesh(
		this->device,
		vertices, indices,
		this->descriptorHeap_CBV_UAV_SRV,
		*filePath);

	// ---------- Get Textures and set them to the mesh START----------
	aiMaterial* mat = assimpScene->mMaterials[assimpMesh->mMaterialIndex];
	
	// Split filepath
	std::string filePathWithoutTexture = *filePath;
	std::size_t indicesInPath = filePathWithoutTexture.find_last_of("/\\");
	filePathWithoutTexture = filePathWithoutTexture.substr(0, indicesInPath + 1);

	// Add the textures to the mesh
	Texture* texture = nullptr;
	for (int i = 0; i < TEXTURE_TYPE::NUM_TEXTURE_TYPES; i++)
	{
		TEXTURE_TYPE type = static_cast<TEXTURE_TYPE>(i);
		texture = ProcessTexture(mat, type, &filePathWithoutTexture);
		mesh->GetMaterial()->SetTexture(type, texture);
	}
	// ---------- Get Textures and set them to the mesh END----------

	// Set shininess
	float shininess = 100;
	// Todo: looks to bright with these values, bad models or bad scene?
	// if (AI_SUCCESS != aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shininess))
	// {
	// 	// if unsuccessful set a default
	// 	shininess = 20.0f;
	// }

	mesh->GetMaterial()->SetShininess(shininess);

	return mesh;
}

Texture* AssetLoader::ProcessTexture(aiMaterial* mat,
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
		defaultPath = this->filePathDefaultTextures + L"default_ambient.png";
		warningMessageTextureType = "Ambient";
		break;
	case::TEXTURE_TYPE::DIFFUSE:
		type = aiTextureType_DIFFUSE;
		defaultPath = this->filePathDefaultTextures + L"default_diffuse.jpg";
		warningMessageTextureType = "Diffuse";
		break;
	case::TEXTURE_TYPE::SPECULAR:
		type = aiTextureType_SPECULAR;
		defaultPath = this->filePathDefaultTextures + L"default_specular.png";
		warningMessageTextureType = "Specular";
		break;
	case::TEXTURE_TYPE::NORMAL:
		type = aiTextureType_NORMALS;
		defaultPath = this->filePathDefaultTextures + L"default_normal.png";
		warningMessageTextureType = "Normal";
		break;
	case::TEXTURE_TYPE::EMISSIVE:
		type = aiTextureType_EMISSIVE;
		defaultPath = this->filePathDefaultTextures + L"default_emissive.png";
		warningMessageTextureType = "Emissive";
		break;
	}

	mat->GetTexture(type, 0, &str);
	std::string textureFile = str.C_Str();
	if (textureFile.size() != 0)
	{
		texture = this->LoadTexture(to_wstring(*filePathWithoutTexture + textureFile).c_str());
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
		return this->loadedTextures[defaultPath].second;
	}

	return nullptr;
}
