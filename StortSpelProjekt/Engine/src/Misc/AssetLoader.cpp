#include "stdafx.h"
#include "AssetLoader.h"

#include "../Renderer/DescriptorHeap.h"
#include "Window.h"

#include "../Renderer/HeightmapModel.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Material.h"
#include "../Renderer/Text.h"
#include "../Renderer/Animation.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "../Renderer/Texture/Texture2D.h"
#include "../Renderer/Texture/Texture2DGUI.h"
#include "../Renderer/Texture/TextureCubeMap.h"

#include "MultiThreading/ThreadPool.h"
#include "MultiThreading/CalculateHeightmapNormalsTask.h"

#include "EngineMath.h"

AssetLoader::AssetLoader(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV, const Window* window)
{
	m_pDevice = device;
	m_pDescriptorHeap_CBV_UAV_SRV = descriptorHeap_CBV_UAV_SRV;
	m_pWindow = const_cast<Window*>(window);

	std::map<TEXTURE2D_TYPE, Texture*> matTextures;
	// Load default textures
	matTextures[TEXTURE2D_TYPE::ALBEDO]		= LoadTexture2D(m_FilePathDefaultTextures + L"default_albedo.dds");
	matTextures[TEXTURE2D_TYPE::ROUGHNESS]	= LoadTexture2D(m_FilePathDefaultTextures + L"default_roughness.dds");
	matTextures[TEXTURE2D_TYPE::METALLIC]	= LoadTexture2D(m_FilePathDefaultTextures + L"default_metallic.dds");
	matTextures[TEXTURE2D_TYPE::NORMAL]		= LoadTexture2D(m_FilePathDefaultTextures + L"default_normal.dds");
	matTextures[TEXTURE2D_TYPE::EMISSIVE]	= LoadTexture2D(m_FilePathDefaultTextures + L"default_emissive.dds");

	std::wstring matName = L"DefaultMaterial";
	Material* material = new Material(&matName, &matTextures);
	m_LoadedMaterials[matName].first = false;
	m_LoadedMaterials[matName].second = material;
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

	// For every Material
	for (auto material : m_LoadedMaterials)
	{
		delete material.second.second;
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

Model* AssetLoader::LoadModel(const std::wstring& path)
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
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to load model with path: \'%S\'\n", path.c_str());
		return nullptr;
	}

	std::vector<Mesh*> meshes;
	std::vector<std::map<TEXTURE2D_TYPE, Texture*>> textures;
	std::vector<Animation*> animations;
	std::vector<Material*> materials;

	meshes.reserve(assimpScene->mNumMeshes);
	materials.reserve(assimpScene->mNumMeshes);
	animations.reserve(assimpScene->mNumAnimations);
	m_LoadedModels[path].first = false;
	

	processNode(assimpScene->mRootNode, assimpScene, &meshes, &materials, path);
	processAnimations(assimpScene, &animations);

	m_LoadedModels[path].second = new Model(&path, &meshes, &animations, &materials);

	return m_LoadedModels[path].second;
}

HeightmapModel* AssetLoader::LoadHeightmap(const std::wstring& path)
{
	// return value.
	HeightmapModel* model = nullptr;

	// Check if the heightmap model already exists
	if (m_LoadedModels.count(path) != 0)
	{
		model = dynamic_cast<HeightmapModel*>(m_LoadedModels[path].second);
		
		if (!model)
		{
			Log::PrintSeverity(Log::Severity::OTHER, "The model %S is already loaded and attempted to be loaded as a HeightmapModel!", path);
		}

		return model;
	}
	std::wstring heightMapPath;
	std::wstring materialPath;
	getHeightMapResources(path, heightMapPath, materialPath);

	Texture* tex = LoadTexture2D(heightMapPath);

	// One dimensional!
	unsigned char* imgData = tex->GetData();
	unsigned int dataCount = tex->GetHeight() * tex->GetWidth();
	std::vector<Vertex> vertices;
	vertices.reserve(dataCount);

	float* heightData = new float[dataCount];

	// Create vertices, only positions and UVs.
	for (unsigned int i = 0; i < dataCount; i++)
	{
		Vertex ver;
		heightData[i] = imgData[i * 4] / 255.0f;
		
		ver.pos = { static_cast<float>(tex->GetWidth() - (i % tex->GetWidth())) - tex->GetWidth() / 2.0f, heightData[i], (tex->GetHeight() - static_cast<float>(i) / tex->GetWidth()) - tex->GetHeight() / 2.0f };
		ver.uv = { static_cast<float>(i % tex->GetWidth()) / tex->GetWidth(), static_cast<float>(i / tex->GetWidth()) / tex->GetHeight() };
		vertices.push_back(ver);
	}

	// Calculate and store indices
	std::vector<unsigned int> indices;
	unsigned int nrOfTriangles = (tex->GetWidth() - 1) * (tex->GetHeight() - 1) * 2;
	unsigned int nrOfIndices = nrOfTriangles * 3;
	unsigned int toProcess = vertices.size() - tex->GetWidth();

	indices.reserve(nrOfIndices);

	for (unsigned int i = 0; i < toProcess; i++)
	{
		// calculate the indices for each triangle.
		// they are set up in the order upper left, lower left and upper right and  for the first triangle. 
		// for the second triangle the indices are set up as lower left, lower right and upper right.

		// First triangle
		indices.push_back(i);
		indices.push_back(i + tex->GetWidth());
		indices.push_back(i + 1);

		// Second triangle
		indices.push_back(i + tex->GetWidth());
		indices.push_back(i + tex->GetWidth() + 1);
		indices.push_back(i + 1);

		// Make sure that we dont create triangles from the border. If so add one more step.
		/*
				Border
				|
			*-*-*
			|/|/|
			*-*-*
			|/|/|
			*-*-*
		*/
		if (((i + 2) % tex->GetWidth()) == 0)
		{
			i++;
		}
	}

	/*
	// Calculate normals
	float3 neighbours[4];
	float3 neighbourTangents[4];
	float3 normals[4];
	float3 verPos;
	float3 vertexNormal;
	// left = 0
	// down = 1
	// right = 2
	// up = 3
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		verPos = { vertices[i].pos.x, vertices[i].pos.y, vertices[i].pos.z };
		// Is left in the grid?
		if (i != 0)
		{
			neighbours[0] = { vertices[i - 1].pos.x, vertices[i - 1].pos.y, vertices[i - 1].pos.z };
		}
		else
		{
			neighbours[0] = { 0,0,0 };
		}

		// is down on the grid?
		if (i / tex->GetWidth() + 1 < tex->GetHeight())
		{
			neighbours[1] = { vertices[i + tex->GetWidth()].pos.x, vertices[i + tex->GetWidth()].pos.y, vertices[i + tex->GetWidth()].pos.z };
		}
		else
		{
			neighbours[1] = { 0,0,0 };
		}

		// Is right on the grid?
		if (i % tex->GetWidth() + 1 < tex->GetWidth())
		{
			neighbours[2] = { vertices[i + 1].pos.x, vertices[i + 1].pos.y, vertices[i + 1].pos.z };
		}
		else
		{
			neighbours[2] = { 0,0,0 };
		}

		// Is up on the grid?
		if (i >= tex->GetWidth())
		{
			neighbours[3] = { vertices[i - tex->GetWidth()].pos.x, vertices[i - tex->GetWidth()].pos.y, vertices[i - tex->GetWidth()].pos.z };
		}
		else
		{
			neighbours[3] = { 0,0,0 };
		}

		// Neighbours calculated. Calculate normals and store them.

		neighbourTangents[0] = neighbours[0] - verPos;
		neighbourTangents[1] = neighbours[1] - verPos;
		neighbourTangents[2] = neighbours[2] - verPos;
		neighbourTangents[3] = neighbours[3] - verPos;

		normals[0] = neighbourTangents[1].cross(&neighbourTangents[0]);
		normals[1] = neighbourTangents[2].cross(&neighbourTangents[1]);
		normals[2] = neighbourTangents[3].cross(&neighbourTangents[2]);
		normals[3] = neighbourTangents[0].cross(&neighbourTangents[3]);

		vertexNormal = normals[0] + normals[1] + normals[2] + normals[3];
		vertexNormal.normalize();

		vertices[i].normal.x = -vertexNormal.x;
		vertices[i].normal.y = -vertexNormal.y;
		vertices[i].normal.z = -vertexNormal.z;
	}

	// Move on to tangents.
	float3 edge1 = { 0,0,0 };
	float3 edge2 = { 0,0,0 };
	float2 deltaUV1 = { 0,0};
	float2 deltaUV2 = { 0,0 };
	//Reuse neighbours array from normals!
	neighbours[0] = { 0 };
	neighbours[1] = { 0 };
	neighbours[2] = { 0 };
	float2 uv[3] = { 0 };
	float f = 0;

	for (unsigned int i = 0; i < nrOfIndices - 3; i++)
	{
		neighbours[0] = { vertices[indices[i]].pos.x,	  vertices[indices[i]].pos.y,	  vertices[indices[i]].pos.z };
		neighbours[1] = { vertices[indices[i + 1]].pos.x, vertices[indices[i + 1]].pos.y, vertices[indices[i + 1]].pos.z };
		neighbours[2] = { vertices[indices[i + 2]].pos.x, vertices[indices[i + 2]].pos.y, vertices[indices[i + 2]].pos.z };
		
		uv[0] = { vertices[indices[i]].uv.x,	 vertices[indices[i]].uv.y	};
		uv[1] = { vertices[indices[i + 1]].uv.x, vertices[indices[i + 1]].uv.y };
		uv[2] = { vertices[indices[i + 2]].uv.x, vertices[indices[i + 2]].uv.y };

		edge1 = neighbours[1] - neighbours[0];
		edge2 = neighbours[2] - neighbours[0];
		deltaUV1 = uv[1] - uv[0];
		deltaUV2 = uv[2] - uv[0];

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		for (unsigned int j = 0; j < 3; j++)
		{
			vertices[indices[i + j]].tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			vertices[indices[i + j]].tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			vertices[indices[i + j]].tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		}
	}
	*/

	CalculateHeightmapNormalsTask firstTask (0, ThreadPool::GetInstance().GetNrOfThreads(), vertices, indices, tex->GetWidth(), tex->GetHeight());

	unsigned int nrOfThreads = ThreadPool::GetInstance().GetNrOfThreads();
	CalculateHeightmapNormalsTask** tasks = new CalculateHeightmapNormalsTask*[nrOfThreads];
	
	for (int i = 0; i < nrOfThreads; i++)
	{
		tasks[i] = new CalculateHeightmapNormalsTask(i, nrOfThreads, vertices, indices, tex->GetWidth(), tex->GetHeight());
		ThreadPool::GetInstance().AddTask(tasks[i]);
	}
	
	ThreadPool::GetInstance().WaitForThreads(firstTask.GetThreadFlags());

	for (int i = 0; i < nrOfThreads; i++)
	{
		delete tasks[i];
	}
	delete[] tasks;

	Mesh* mesh = new Mesh(m_pDevice, &vertices, &indices, m_pDescriptorHeap_CBV_UAV_SRV, path);

	m_LoadedMeshes.push_back(mesh);

	std::vector<Mesh*> meshes;
	meshes.push_back(mesh);

	std::vector<Animation*> animations;
	std::vector<Material*> materials;
	materials.push_back(loadMaterialFromMTL(materialPath));
	model = new HeightmapModel(&path, &meshes, &animations, &materials, heightData);
	m_LoadedModels[path].first = false;
	m_LoadedModels[path].second = model;

	return model;
}

Texture* AssetLoader::LoadTexture2D(const std::wstring& path)
{
	// Check if the texture already exists
	if (m_LoadedTextures.count(path) != 0)
	{
		return m_LoadedTextures[path].second;
	}

	// Check if the texture is DDS or of other commonType
	std::string fileEnding = GetFileExtension(to_string(path));
	Texture* texture = nullptr;
	if (fileEnding == "dds")
	{
		texture = new Texture2D();
	}
	else
	{
		texture = new Texture2DGUI();
	}

	if (texture->Init(path, m_pDevice, m_pDescriptorHeap_CBV_UAV_SRV) == false)
	{
		delete texture;
		return nullptr;
	}

	m_LoadedTextures[path].first = false;
	m_LoadedTextures[path].second = texture;
	return texture;
}

TextureCubeMap* AssetLoader::LoadTextureCubeMap(const std::wstring& path)
{
	// Check if the texture already exists
	if (m_LoadedTextures.count(path) != 0)
	{
		return static_cast<TextureCubeMap*>(m_LoadedTextures[path].second);
	}

	TextureCubeMap* textureCubeMap = new TextureCubeMap();
	if (textureCubeMap->Init(path, m_pDevice, m_pDescriptorHeap_CBV_UAV_SRV) == false)
	{
		delete textureCubeMap;
		return nullptr;
	}

	m_LoadedTextures[path].first = false;
	m_LoadedTextures[path].second = textureCubeMap;
	return textureCubeMap;
}

std::pair<Font*, Texture*> AssetLoader::LoadFontFromFile(const std::wstring& fontName)
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
		Log::PrintSeverity(Log::Severity::WARNING, "Could not init the font texture for %s.\n", fontName.c_str());
		delete texture;
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

Shader* AssetLoader::loadShader(const std::wstring& fileName, ShaderType type)
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

void AssetLoader::processNode(aiNode* node, const aiScene* assimpScene, std::vector<Mesh*>* meshes, std::vector<Material*>* materials, const std::wstring& filePath)
{
	// Go through all the m_Meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = assimpScene->mMeshes[node->mMeshes[i]];
		meshes->push_back(processMesh(mesh, assimpScene, meshes, materials, filePath));
	}
	
	// If the node has more node children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], assimpScene, meshes, materials, filePath);
	}
}

Mesh* AssetLoader::processMesh(aiMesh* assimpMesh, const aiScene* assimpScene, std::vector<Mesh*>* meshes, std::vector<Material*>* materials, const std::wstring& filePath)
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
		filePath);

	// save mesh
	m_LoadedMeshes.push_back(mesh);

	// Split filepath
	std::wstring filePathWithoutTexture = filePath;
	std::size_t indicesInPath = filePathWithoutTexture.find_last_of(L"/\\");
	filePathWithoutTexture = filePathWithoutTexture.substr(0, indicesInPath + 1);

	// Get material from assimp
	aiMaterial* mat = assimpScene->mMaterials[assimpMesh->mMaterialIndex];
	Material* material;
	// Create our material
	material = loadMaterial(mat, filePathWithoutTexture);
	// add the texture to the correct mesh (later for models slotinfo)
	materials->push_back(material);

	// Set shininess
	// float shininess = 100;
	// Todo: looks to bright with these values, bad models or bad scene?
	// if (AI_SUCCESS != aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shininess))
	// {
	// 	// if unsuccessful set a default
	// 	shininess = 20.0f;
	// }

	return mesh;
}

Material* AssetLoader::loadMaterial(aiMaterial* mat, const std::wstring& folderPath)
{
	// Get material name
	aiString tempName;
	mat->Get(AI_MATKEY_NAME, tempName);
	std::wstring matName = to_wstring(tempName.C_Str());

	// Check if material don't exists
	if (m_LoadedMaterials.count(matName) == 0)
	{
		// Load material
		std::map<TEXTURE2D_TYPE, Texture*> matTextures;

		// Add the textures to the m_pMesh
		for (unsigned int i = 0; i < static_cast<unsigned int>(TEXTURE2D_TYPE::NUM_TYPES); i++)
		{
			TEXTURE2D_TYPE type = static_cast<TEXTURE2D_TYPE>(i);
			Texture* texture = processTexture(mat, type, folderPath);
			matTextures[type] = texture;
		}

		Material* material = new Material(&matName, &matTextures);
		m_LoadedMaterials[matName].first = false;
		m_LoadedMaterials[matName].second = material;

		return material;
	}
	else
	{
		// Don't print for default material
		if (matName != L"DefaultMaterial")
		{
			Log::PrintSeverity(Log::Severity::WARNING, "AssetLoader: Loaded same material name more than once, first loaded material will be used <%S>\n", matName.c_str());
		}
		return m_LoadedMaterials[matName].second;
	}
}

Material* AssetLoader::loadMaterialFromMTL(const std::wstring& path)
{
	std::wifstream ifstream(path);
	Material* mat = nullptr;
	if (ifstream.is_open())
	{
		std::wstring relPath = path.substr(0, path.find_last_of('/') + 1);
		std::wstring currMatName;
		std::wstring line;
		std::wstring varName;
		std::wstring varVal;
		std::map<TEXTURE2D_TYPE, Texture*> matTextures;

		std::vector<std::wstring> defaultNames;
		defaultNames.reserve(static_cast<unsigned int>(TEXTURE2D_TYPE::NUM_TYPES));
		defaultNames.push_back(L"default_albedo.dds");
		defaultNames.push_back(L"default_roughness.dds");
		defaultNames.push_back(L"default_metallic.dds");
		defaultNames.push_back(L"default_normal.dds");
		defaultNames.push_back(L"default_emissive.dds");

		for (unsigned int i = 0; i < static_cast<unsigned int>(TEXTURE2D_TYPE::NUM_TYPES); i++)
		{
			matTextures[static_cast<TEXTURE2D_TYPE>(i)] = m_LoadedTextures[m_FilePathDefaultTextures + defaultNames[i]].second;
		}

		while (!ifstream.eof())
		{
			std::getline(ifstream, line);
			varName = line.substr(0, line.find_first_of(L' '));

			if (varName == L"newmtl")
			{
				currMatName = line.substr(line.find_first_of(L' '));
				if (m_LoadedMaterials.count(currMatName) > 0)
				{
					ifstream.close();
					return m_LoadedMaterials[currMatName].second;
				}
			}
			else if (varName == L"map_Ka")
			{
				varVal = line.substr(line.find_first_of(L' ') + 1);
				matTextures[TEXTURE2D_TYPE::METALLIC] = LoadTexture2D(relPath + varVal);
			}
			else if (varName == L"map_Kd")
			{
				varVal = line.substr(line.find_first_of(L' ') + 1);
				matTextures[TEXTURE2D_TYPE::ALBEDO] = LoadTexture2D(relPath + varVal);
			}
			else if (varName == L"map_Ks")
			{
				varVal = line.substr(line.find_first_of(L' ') + 1);
				matTextures[TEXTURE2D_TYPE::ROUGHNESS] = LoadTexture2D(relPath + varVal);
			}
			else if (varName == L"map_Kn")
			{
				varVal = line.substr(line.find_first_of(L' ') + 1);
				matTextures[TEXTURE2D_TYPE::NORMAL] = LoadTexture2D(relPath + varVal);
			}
			else if (varName == L"map_Ke")
			{
				varVal = line.substr(line.find_first_of(L' ') + 1);
				matTextures[TEXTURE2D_TYPE::EMISSIVE] = LoadTexture2D(relPath + varVal);
			}
		}

		mat = new Material(&currMatName, &matTextures);
		m_LoadedMaterials[currMatName].first = false;
		m_LoadedMaterials[currMatName].second = mat;

	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Could not open mtl file with path %S", path.c_str());
	}

	return mat;
}

Texture* AssetLoader::processTexture(aiMaterial* mat, TEXTURE2D_TYPE texture_type, const std::wstring& filePathWithoutTexture)
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
	case::TEXTURE2D_TYPE::ALBEDO:
		type = aiTextureType_DIFFUSE;
		defaultPath = m_FilePathDefaultTextures + L"default_albedo.dds";
		warningMessageTextureType = "Albedo";
		break;
	case::TEXTURE2D_TYPE::ROUGHNESS:
		type = aiTextureType_SPECULAR;
		defaultPath = m_FilePathDefaultTextures + L"default_roughness.dds";
		warningMessageTextureType = "Roughness";
		break;
	case::TEXTURE2D_TYPE::METALLIC:
		type = aiTextureType_AMBIENT;
		defaultPath = m_FilePathDefaultTextures + L"default_metallic.dds";
		warningMessageTextureType = "Metallic";
		break;
	case::TEXTURE2D_TYPE::NORMAL:
		type = aiTextureType_NORMALS;
		defaultPath = m_FilePathDefaultTextures + L"default_normal.dds";
		warningMessageTextureType = "Normal";
		break;
	case::TEXTURE2D_TYPE::EMISSIVE:
		type = aiTextureType_EMISSIVE;
		defaultPath = m_FilePathDefaultTextures + L"default_emissive.dds";
		warningMessageTextureType = "Emissive";
		break;
	}

	mat->GetTexture(type, 0, &str);
	std::wstring textureFile = to_wstring(str.C_Str());
	if (textureFile.size() != 0)
	{
		texture = LoadTexture2D(filePathWithoutTexture + textureFile);
	}

	if (texture != nullptr)
	{
		return texture;
	}
	else
	{
		std::string tempString = std::string(filePathWithoutTexture.begin(), filePathWithoutTexture.end());
		// No texture, warn and apply default Texture
		Log::PrintSeverity(Log::Severity::WARNING, "Applying default texture: " + warningMessageTextureType +
			" on mesh with path: \'%s\'\n", tempString.c_str());
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

void AssetLoader::getHeightMapResources(const std::wstring& path, std::wstring& heightMapPath, std::wstring& materialPath)
{
	std::wifstream input(path);
	
	if (input.is_open())
	{
		std::wstring relFolderPath = path.substr(0, path.find_last_of('/') + 1);
		std::getline(input, heightMapPath);
		heightMapPath = relFolderPath + heightMapPath;
		std::getline(input, materialPath);
		materialPath = relFolderPath + materialPath;
		input.close();
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Could not load heightmap info file with path %S", path.c_str());
	}
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
