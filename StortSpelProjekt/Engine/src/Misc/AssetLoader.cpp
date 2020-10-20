#include "stdafx.h"
#include "AssetLoader.h"

#include "../ECS/Scene.h"
#include "../ECS/Entity.h"

#include "../Renderer/DescriptorHeap.h"
#include "Window.h"
#include "../Renderer/HeightmapModel.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Material.h"
#include "../Renderer/Animation.h"
#include "../Renderer/Transform.h"

#include "../Misc/GUI2DElements/Text.h"
#include "../Misc/GUI2DElements/Font.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "../Renderer/Texture/Texture2D.h"
#include "../Renderer/Texture/Texture2DGUI.h"
#include "../Renderer/Texture/TextureCubeMap.h"

#include "MultiThreading/ThreadPool.h"
#include "MultiThreading/CalculateHeightmapNormalsTask.h"

#include "EngineMath.h"

#include "../Misc/NavMesh.h"

AssetLoader::AssetLoader(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV, const Window* window)
{
	m_pDevice = device;
	m_pDescriptorHeap_CBV_UAV_SRV = descriptorHeap_CBV_UAV_SRV;
	m_pWindow = const_cast<Window*>(window);

	// Load default textures
	loadDefaultMaterial();
}

bool AssetLoader::IsModelLoadedOnGpu(const std::wstring& name) const
{
	return m_LoadedModels.at(name).first;
}

bool AssetLoader::IsModelLoadedOnGpu(const Model* model) const
{
	return m_LoadedModels.at(model->GetPath()).first;
}

bool AssetLoader::IsMaterialLoadedOnGpu(const std::wstring& name) const
{
	return m_LoadedMaterials.at(name).first;
}

bool AssetLoader::IsMaterialLoadedOnGpu(const Material* material) const
{
	return m_LoadedMaterials.at(material->GetPath()).first;
}

bool AssetLoader::IsTextureLoadedOnGpu(const std::wstring& name) const
{
	return m_LoadedTextures.at(name).first;
}

bool AssetLoader::IsTextureLoadedOnGpu(const Texture* texture) const
{
	return m_LoadedTextures.at(texture->GetPath()).first;
}

void AssetLoader::loadDefaultMaterial()
{
	// Load default textures
	std::map<TEXTURE2D_TYPE, Texture*> matTextures;
	matTextures[TEXTURE2D_TYPE::ALBEDO] = LoadTexture2D(m_FilePathDefaultTextures + L"default_albedo.dds");
	matTextures[TEXTURE2D_TYPE::ROUGHNESS] = LoadTexture2D(m_FilePathDefaultTextures + L"default_roughness.dds");
	matTextures[TEXTURE2D_TYPE::METALLIC] = LoadTexture2D(m_FilePathDefaultTextures + L"default_metallic.dds");
	matTextures[TEXTURE2D_TYPE::NORMAL] = LoadTexture2D(m_FilePathDefaultTextures + L"default_normal.dds");
	matTextures[TEXTURE2D_TYPE::EMISSIVE] = LoadTexture2D(m_FilePathDefaultTextures + L"default_emissive.dds");

	std::wstring matName = L"DefaultMaterial";
	Material* material = new Material(&matName, &matTextures);
	m_LoadedMaterials[matName].first = false;
	m_LoadedMaterials[matName].second = material;
}

bool AssetLoader::IsFontTextureLoadedOnGPU(const Font* font) const
{
	const std::wstring path = m_FilePathFonts + font->GetName() + L".fnt";
	return m_LoadedFonts.at(path).first;
}

AssetLoader::~AssetLoader()
{
	// For every Mesh
	for (auto mesh : m_LoadedMeshes)
	{
		delete mesh;
	}

	// For every texture
	for (auto pair : m_LoadedTextures)
	{
		delete pair.second.second;
	}

	// For every Material
	for (auto material : m_LoadedMaterials)
	{
		delete material.second.second;
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

	// For every shader
	for (auto shader : m_LoadedShaders)
	{
		delete shader.second;
	}

	// For every font
	for (auto font : m_LoadedFonts)
	{
		delete font.second.second->m_pKerningsList;
		delete font.second.second->m_pCharList;
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

	const aiScene* assimpScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded | aiProcess_OptimizeMeshes);

	if (assimpScene == nullptr)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to load model with path: \'%S\'\n", path.c_str());
		return nullptr;
	}

	std::vector<Mesh*> meshes;
	std::vector<std::map<TEXTURE2D_TYPE, Texture*>> textures;
	std::vector<Material*> materials;

	meshes.reserve(assimpScene->mNumMeshes);
	materials.reserve(assimpScene->mNumMeshes);
	processNode(assimpScene->mRootNode, assimpScene, &meshes, &materials, path);
	//Log::Print("\n\n\n");

	// Animation stuff
	std::vector<Animation*> animations;
	animations.reserve(assimpScene->mNumAnimations);
	std::map<unsigned int, VertexWeight> perVertexBoneData;
	std::map<std::string, BoneInfo> boneCounter;

	SkeletonNode* rootNode = processSkeleton(boneCounter, assimpScene->mRootNode, assimpScene, &perVertexBoneData);
	processAnimations(assimpScene, &animations);
	if (!animations.empty())
	{
		initializeSkeleton(rootNode, boneCounter, animations[0]);	// Ugly solution, should not pass animation[0].
	}
	// End of animation stuff

	m_LoadedModels[path].second = new Model(&path, rootNode, &perVertexBoneData, &meshes, &animations, &materials);
	//m_LoadedModels[path].second->updateSlotInfo();
	m_LoadedModels[path].first = false;

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
	unsigned int dataCount = tex->GetWidth() * tex->GetHeight();
	std::vector<Vertex> vertices;
	vertices.reserve(dataCount);

	double* heightData = new double[dataCount];

	// Create vertices, only positions and UVs.
	for (unsigned int i = 0; i < dataCount; i++)
	{
		Vertex ver;
		heightData[i] = imgData[i * 4] / 255.0f;
		
		ver.pos = { static_cast<float>(i % tex->GetHeight()) - tex->GetHeight() / 2.0f, static_cast<float>(heightData[i]), (i / tex->GetHeight()) - tex->GetWidth() / 2.0f };
		ver.uv = { static_cast<float>(i % tex->GetHeight()) / tex->GetHeight(), static_cast<float>(i / tex->GetHeight()) / tex->GetWidth() };
		vertices.push_back(ver);
	}

	// Calculate and store indices
	std::vector<unsigned int> indices;
	unsigned int nrOfTriangles = (tex->GetHeight() - 1) * (tex->GetWidth() - 1) * 2;
	unsigned int nrOfIndices = nrOfTriangles * 3;
	unsigned int toProcess = vertices.size() - tex->GetHeight();

	indices.reserve(nrOfIndices);

	for (unsigned int i = 0; i < toProcess; i++)
	{
		// calculate the indices for each triangle.
		// they are set up in the order upper left, lower left and upper right and  for the first triangle.
		// for the second triangle the indices are set up as lower left, lower right and upper right.

		// First triangle
		indices.push_back(i);
		indices.push_back(i + tex->GetHeight());
		indices.push_back(i + 1);

		// Second triangle
		indices.push_back(i + tex->GetHeight());
		indices.push_back(i + tex->GetHeight() + 1);
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

		i+= (((i + 2) % tex->GetHeight()) == 0);
	}

	unsigned int nrOfThreads = ThreadPool::GetInstance().GetNrOfThreads();
	CalculateHeightmapNormalsTask** tasks = new CalculateHeightmapNormalsTask*[nrOfThreads];
	
	for (int i = 0; i < nrOfThreads; i++)
	{
		tasks[i] = new CalculateHeightmapNormalsTask(i, nrOfThreads, vertices, indices, tex->GetHeight(), tex->GetWidth());
		ThreadPool::GetInstance().AddTask(tasks[i]);
	}
	
	ThreadPool::GetInstance().WaitForThreads(tasks[0]->GetThreadFlags());

	for (int i = 0; i < nrOfThreads; i++)
	{
		delete tasks[i];
	}
	delete[] tasks;

	Mesh* mesh = new Mesh(m_pDevice, &vertices, &indices, m_pDescriptorHeap_CBV_UAV_SRV, path);
	mesh->Init(m_pDevice, m_pDescriptorHeap_CBV_UAV_SRV);
	m_LoadedMeshes.push_back(mesh);

	std::vector<Mesh*> meshes;
	meshes.push_back(mesh);

	SkeletonNode* rootNode = nullptr;
	std::map<unsigned int, VertexWeight> PVBD;
	std::vector<Animation*> animations;
	std::vector<Material*> materials;
	materials.push_back(loadMaterialFromMTL(materialPath));
	model = new HeightmapModel(&path, rootNode, &PVBD, &meshes, &animations, &materials, heightData, static_cast<double>(tex->GetHeight()), static_cast<double>(tex->GetWidth()));
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
		texture = new Texture2D(path);
	}
	else
	{
		texture = new Texture2DGUI(path);
	}

	m_LoadedTextures[path].first = false;
	m_LoadedTextures[path].second = texture;

	// Create dx resources etc..
	texture->Init(m_pDevice, m_pDescriptorHeap_CBV_UAV_SRV);

	return texture;
}

TextureCubeMap* AssetLoader::LoadTextureCubeMap(const std::wstring& path)
{
	// Check if the texture already exists
	if (m_LoadedTextures.count(path) != 0)
	{
		return static_cast<TextureCubeMap*>(m_LoadedTextures[path].second);
	}

	TextureCubeMap* textureCubeMap = new TextureCubeMap(path);

	m_LoadedTextures[path].first = false;
	m_LoadedTextures[path].second = textureCubeMap;

	// load to vram
	textureCubeMap->Init(m_pDevice, m_pDescriptorHeap_CBV_UAV_SRV);

	return textureCubeMap;
}

Font* AssetLoader::LoadFontFromFile(const std::wstring& fontName)
{
	const std::wstring path = m_FilePathFonts + fontName;

	// Check if the font already exists
	if (m_LoadedFonts.count(path) != 0)
	{
		return m_LoadedFonts[path].second;
	}

	// else load the font and store it in m_LoadedFonts
	m_LoadedFonts[path].second = loadFont(path.c_str(), m_pWindow->GetScreenWidth(), m_pWindow->GetScreenHeight());

	// and create the texture
	Texture* texture = LoadTexture2D(m_LoadedFonts[path].second->m_FontImage);
	if (texture == nullptr)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Could not init the font texture for %s.\n", fontName.c_str());
		delete texture;
		return m_LoadedFonts[path].second;
	}
	m_LoadedFonts[path].first = false;
	m_LoadedFonts[path].second->m_pTexture = texture;

	return m_LoadedFonts[path].second;
}

void AssetLoader::LoadMap(Scene* scene, const char* path)
{
	FILE* file = fopen(path, "r");

	std::string lineHeader;
	lineHeader.reserve(128);
	std::string entityName;
	entityName.reserve(128);
	std::string modelPath;
	modelPath.reserve(128);
	std::string toSubmit;
	toSubmit.reserve(128);
	unsigned int flag;
	unsigned int flagVal;
	bool drawFlags[FLAG_DRAW::NUM_FLAG_DRAWS] = { 0 };
	unsigned int combinedFlag = 0;
	float3 scaling = { 1.0f,1.0f,1.0f };
	float3 pos = { 0.0, 0.0, 0.0 };
	float3 rot = { 0.0, 0.0, 0.0};
	float3 lightColor = { 0.0, 0.0, 0.0 };
	float3 lightDir = { 0.0, 0.0, 0.0 };
	float3 lightAttenuation = { 0.0, 0.0, 0.0 };
	float lightAspect = 16.0f / 9.0f;
	float lightCutOff = 30.0f;
	float lightOuterCutOff = 45.0f;
	float lightNear = 0.01;
	float lightFar = 1000.0;
	float lightLeft = -30.0;
	float lightRight = 30.0;
	float lightTop = 30.0;
	float lightBottom = -30.0;
	bool lightFlags[FLAG_LIGHT::NUM_FLAGS_LIGHT] = { 0 };
	unsigned int collisionComponent = 0;
	HeightmapModel* heightmapModel = nullptr;
	float3 shapeInfo = { 0.0f,0.0f,0.0f };
	float mass = 0.0f;
	float friction = 0.0f;
	float restitution = 0.0f;
	HeightMapInfo hmInfo;
	std::string fullPath;
	fullPath.reserve(256);
	float2 size = { 0.0, 0.0 };
	int quad1 = 0;
	int quad2 = 0;

	NavMesh* navMesh;

	component::ModelComponent* mc = nullptr;
	component::TransformComponent* tc = nullptr;
	component::PointLightComponent* plc = nullptr;
	component::SpotLightComponent* slc = nullptr;
	component::DirectionalLightComponent* dlc = nullptr;
	component::CollisionComponent* cc = nullptr;
	component::SkyboxComponent* sbc = nullptr;
	Entity* entity = nullptr;
	if (file != NULL)
	{
		while (fscanf(file, "%s", lineHeader.c_str()) != EOF)
		{
			if (strcmp(lineHeader.c_str(), "Name") == 0)
			{
				fscanf(file, "%s", entityName.c_str());
				entity = scene->AddEntity(entityName.c_str());
			}
			else if (strcmp(lineHeader.c_str(), "NavMesh") == 0)
			{
				scene->CreateNavMesh();
				navMesh = scene->GetNavMesh();
			}
			else if (strcmp(lineHeader.c_str(), "ModelPath") == 0)
			{
				fscanf(file, "%s", modelPath.c_str());
				fullPath = path;
				fullPath = fullPath.substr(0, fullPath.find_last_of("/") + 1).c_str();
				fullPath += modelPath.c_str();
			}
			else if (strcmp(lineHeader.c_str(), "ModelScaling") == 0)
			{
				fscanf(file, "%f,%f,%f", &scaling.x, &scaling.y, &scaling.z);
			}
			else if (strcmp(lineHeader.c_str(), "ModelRotation") == 0)
			{
				fscanf(file, "%f,%f,%f", &rot.x, &rot.y, &rot.z);
			}
			else if (strcmp(lineHeader.c_str(), "ModelPosition") == 0 || strcmp(lineHeader.c_str(), "NavQuadPosition") == 0)
			{
				fscanf(file, "%f,%f,%f", &pos.x, &pos.y, &pos.z);
			}
			else if (strcmp(lineHeader.c_str(), "ModelDrawFlag") == 0)
			{
				fscanf(file, "%d,%d", &flag, &flagVal);
				drawFlags[flag] = flagVal;
			}
			else if (strcmp(lineHeader.c_str(), "ModelLightFlag") == 0)
			{
				fscanf(file, "%d,%d", &flag, &flagVal);
				lightFlags[flag] = flagVal;
			}
			else if (strcmp(lineHeader.c_str(), "ModelLightColor") == 0)
			{
				fscanf(file, "%f,%f,%f", &lightColor.x, &lightColor.y, &lightColor.z);
			}
			else if (strcmp(lineHeader.c_str(), "ModelLightDirection") == 0)
			{
				fscanf(file, "%f,%f,%f", &lightDir.x, &lightDir.y, &lightDir.z);
			}
			else if (strcmp(lineHeader.c_str(), "ModelLightAttenuation") == 0)
			{
				fscanf(file, "%f,%f,%f", &lightAttenuation.x, &lightAttenuation.y, &lightAttenuation.z);
			}
			else if (strcmp(lineHeader.c_str(), "ModelLightAspectRatio") == 0)
			{
				fscanf(file, "%f", &lightAspect);
			}
			else if (strcmp(lineHeader.c_str(), "ModelLightCutOff") == 0)
			{
				fscanf(file, "%f", &lightCutOff);
			}
			else if (strcmp(lineHeader.c_str(), "ModelLightOuterCutOff") == 0)
			{
				fscanf(file, "%f", &lightOuterCutOff);
			}
			else if (strcmp(lineHeader.c_str(), "ModelLightNear") == 0)
			{
				fscanf(file, "%f", &lightNear);
			}
			else if (strcmp(lineHeader.c_str(), "ModelLightFar") == 0)
			{
				fscanf(file, "%f", &lightFar);
			}
			else if (strcmp(lineHeader.c_str(), "ModelLightLeft") == 0)
			{
				fscanf(file, "%f", &lightLeft);
			}
			else if (strcmp(lineHeader.c_str(), "ModelLightRight") == 0)
			{
				fscanf(file, "%f", &lightRight);
			}
			else if (strcmp(lineHeader.c_str(), "ModelLightTop") == 0)
			{
				fscanf(file, "%f", &lightTop);
			}
			else if (strcmp(lineHeader.c_str(), "ModelLightBottom") == 0)
			{
				fscanf(file, "%f", &lightBottom);
			}
			else if (strcmp(lineHeader.c_str(), "ModelMass") == 0)
			{
				fscanf(file, "%f", &mass);
			}
			else if (strcmp(lineHeader.c_str(), "ModelFriction") == 0)
			{
				fscanf(file, "%f", &friction);
			}
			else if (strcmp(lineHeader.c_str(), "ModelRestitution") == 0)
			{
				fscanf(file, "%f", &restitution);
			}
			else if (strcmp(lineHeader.c_str(), "NavQuadSize") == 0)
			{
				fscanf(file, "%f,%f", &size.x, &size.y);
			}
			else if (strcmp(lineHeader.c_str(), "NavConnectionQuads") == 0)
			{
				fscanf(file, "%d,%d", &quad1, &quad2);
			}
			else if (strcmp(lineHeader.c_str(), "Submit") == 0)
			{
				fscanf(file, "%s", toSubmit.c_str());

				if (strcmp(toSubmit.c_str(), "Model") == 0)
				{	
					mc = entity->AddComponent<component::ModelComponent>();
					tc = entity->AddComponent<component::TransformComponent>();
					tc->GetTransform()->SetScale(1.0f);
					tc->GetTransform()->SetScale(scaling.x,scaling.y,scaling.z);
					tc->GetTransform()->SetRotationX(rot.x);
					tc->GetTransform()->SetRotationY(rot.y);
					tc->GetTransform()->SetRotationZ(rot.z);
					tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);

					mc->SetModel(AssetLoader::LoadModel(to_wstring(fullPath)));
					combinedFlag = 0;
					for (int i = 0; i < FLAG_DRAW::NUM_FLAG_DRAWS; ++i)
					{
						combinedFlag |= BIT(i + 1) * drawFlags[i];
					}

					mc->SetDrawFlag(combinedFlag);
				}
				else if (strcmp(toSubmit.c_str(), "Heightmap") == 0)
				{
					mc = entity->AddComponent<component::ModelComponent>();
					heightmapModel = AssetLoader::LoadHeightmap(to_wstring(fullPath));
					mc->SetModel(heightmapModel);
					combinedFlag = 0;
					for (int i = 0; i < FLAG_DRAW::NUM_FLAG_DRAWS; ++i)
					{
						combinedFlag += BIT(i + 1) * drawFlags[i];
					}
					mc->SetDrawFlag(combinedFlag);
					tc = entity->AddComponent<component::TransformComponent>();
					tc->GetTransform()->SetRotationX(rot.x);
					tc->GetTransform()->SetRotationY(rot.y);
					tc->GetTransform()->SetRotationZ(rot.z);
					tc->GetTransform()->SetScale(scaling.x, scaling.y, scaling.z);
					tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);
				}
				else if (strcmp(toSubmit.c_str(), "PointLight") == 0)
				{
					combinedFlag = 0;
					for (int i = 0; i < FLAG_LIGHT::NUM_FLAGS_LIGHT; ++i)
					{
						combinedFlag |= BIT(i + 1) * lightFlags[i];
					}
					plc = entity->AddComponent<component::PointLightComponent>(combinedFlag);
					plc->SetColor(lightColor);
					plc->SetAttenuation(lightAttenuation);
					plc->SetPosition({ pos.x, pos.y, pos.z });
				}
				else if (strcmp(toSubmit.c_str(), "SpotLight") == 0)
				{
					combinedFlag = 0;
					for (int i = 0; i < FLAG_LIGHT::NUM_FLAGS_LIGHT; ++i)
					{
						combinedFlag |= BIT(i + 1) * lightFlags[i];
					}
					slc = entity->AddComponent<component::SpotLightComponent>(combinedFlag);
					slc->SetColor(lightColor);
					slc->SetAttenuation(lightAttenuation);
					slc->SetDirection(lightDir);
					slc->SetPosition({ pos.x, pos.y, pos.z });
					slc->SetAspectRatio(lightAspect);
					slc->SetCutOff(lightCutOff);
					slc->SetOuterCutOff(lightOuterCutOff);
					slc->SetNearPlaneDistance(lightNear);
					slc->SetFarPlaneDistance(lightFar);
					lightAspect = 16.0f / 9.0f;
					lightCutOff = 30.0f;
					lightOuterCutOff = 45.0f;
					lightNear = 0.01;
					lightFar = 1000.0;
				}
				else if (strcmp(toSubmit.c_str(), "DirectionalLight") == 0)
				{
					combinedFlag = 0;
					for (int i = 0; i < FLAG_LIGHT::NUM_FLAGS_LIGHT; ++i)
					{
						combinedFlag |= BIT(i + 1) * lightFlags[i];
					}
					dlc = entity->AddComponent<component::DirectionalLightComponent>(combinedFlag);
					dlc->SetColor(lightColor);
					dlc->SetDirection(lightDir);
					dlc->SetCameraLeft(lightLeft);
					dlc->SetCameraRight(lightRight);
					dlc->SetCameraTop(lightTop);
					dlc->SetCameraBot(lightBottom);
					dlc->SetCameraFarZ(lightFar);
					dlc->SetCameraNearZ(lightNear);
					lightNear = 0.01;
					lightFar = 1000.0;
					lightLeft = -30.0;
					lightRight = 30.0;
					lightTop = 30.0;
					lightBottom = -30.0;
				}
				else if (strcmp(toSubmit.c_str(), "CollisionSphere") == 0)
				{
					fscanf(file, "%f", &shapeInfo.x);
					if (shapeInfo == float3({ 0.0, 0.0, 0.0 }))
					{
						shapeInfo.x = entity->GetComponent<component::ModelComponent>()->GetModelDim().y / 2.0;
					}
					cc = entity->AddComponent<component::SphereCollisionComponent>(mass, shapeInfo.x, friction, restitution);
					shapeInfo = { 0.0f, 0.0f, 0.0f };
					mass = 0.0;
				}
				else if (strcmp(toSubmit.c_str(), "CollisionCapsule") == 0)
				{
					fscanf(file, "%f,%f", &shapeInfo.x, &shapeInfo.y);
					if (shapeInfo == float3({ 0.0, 0.0, 0.0 }))
					{
						shapeInfo.x = entity->GetComponent<component::ModelComponent>()->GetModelDim().z / 2.0;
						shapeInfo.y = entity->GetComponent<component::ModelComponent>()->GetModelDim().y - (shapeInfo.x * 2.0);
					}
					cc = entity->AddComponent<component::CapsuleCollisionComponent>(mass, shapeInfo.x, shapeInfo.y, friction, restitution);
					shapeInfo = { 0.0f, 0.0f, 0.0f };
					mass = 0.0;
				}
				else if (strcmp(toSubmit.c_str(), "CollisionCube") == 0)
				{
					fscanf(file, "%f,%f,%f", &shapeInfo.x, &shapeInfo.y, &shapeInfo.z);
					if (shapeInfo == float3({ 0.0, 0.0, 0.0 }))
					{
						shapeInfo =
						{
							static_cast<float>(entity->GetComponent<component::ModelComponent>()->GetModelDim().x / 2.0),
							static_cast<float>(entity->GetComponent<component::ModelComponent>()->GetModelDim().y / 2.0),
							static_cast<float>(entity->GetComponent<component::ModelComponent>()->GetModelDim().z / 2.0),
						};
					}
					cc = entity->AddComponent<component::CubeCollisionComponent>(mass, shapeInfo.x, shapeInfo.y, shapeInfo.z, friction, restitution);
					shapeInfo = { 0.0f, 0.0f, 0.0f };
					mass = 0.0;
				}
				else if (strcmp(toSubmit.c_str(), "CollisionHeightmap") == 0)
				{
					HeightMapInfo info;
					info.data = heightmapModel->GetHeights();
					info.length = heightmapModel->GetLength();
					info.width = heightmapModel->GetWidth();
					info.maxHeight = 1;
					info.minHeight = -1;
					// Implement when feature is merged to develop
					cc = entity->AddComponent<component::HeightmapCollisionComponent>(info,mass,friction,restitution);
					mass = 0.0;
				}
				else if (strcmp(toSubmit.c_str(), "NavQuad") == 0)
				{
					navMesh->AddNavQuad(pos, size);
				}
				else if (strcmp(toSubmit.c_str(), "NavConnection") == 0)
				{
					navMesh->ConnectNavQuads(quad1, quad2);
				}
				else if (strcmp(toSubmit.c_str(), "NavMesh") == 0)
				{
					navMesh->CreateGrid();
				}
				else if (strcmp(toSubmit.c_str(), "Skybox") == 0)
				{
					TextureCubeMap* skyboxCubemap = AssetLoader::LoadTextureCubeMap(to_wstring(fullPath));
					sbc = entity->AddComponent<component::SkyboxComponent>();
					sbc->SetTexture(skyboxCubemap);
				}
			}
		}
		fclose(file);
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Could not load mapfile %s", path);
	}
}

std::wstring AssetLoader::GetFontPath() const
{
	return m_FilePathFonts;
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
	/*static int level = 0;
	if (node->mTransformation.IsIdentity())
		Log::Print("aiNode: %s I\n", node->mName.C_Str());
	else
		Log::Print("aiNode: %s NOT I\n", node->mName.C_Str());
	*/

	// Go through all the m_Meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = assimpScene->mMeshes[node->mMeshes[i]];
		meshes->push_back(processMesh(mesh, assimpScene, meshes, materials, filePath));
	}
	
	// If the node has more node children
	//if (node->mNumChildren != 0)
	//	level++;

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		//for (int j = 0; j < level; j++)
		//	Log::Print("\t");
		processNode(node->mChildren[i], assimpScene, meshes, materials, filePath);
	}

	//if (node->mNumChildren != 0)
	//	level--;
}

Mesh* AssetLoader::processMesh(aiMesh* assimpMesh, const aiScene* assimpScene, std::vector<Mesh*>* meshes, std::vector<Material*>* materials, const std::wstring& filePath)
{
	// Fill this data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
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

	// Create Mesh
	Mesh* mesh = new Mesh(
		m_pDevice,
		&vertices, &indices,
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


	mesh->Init(m_pDevice, m_pDescriptorHeap_CBV_UAV_SRV);

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
			//Log::PrintSeverity(Log::Severity::WARNING, "AssetLoader: Loaded same material name more than once, first loaded material will be used <%S>\n", matName.c_str());
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
#ifdef _DEBUG
		std::string tempString = std::string(filePathWithoutTexture.begin(), filePathWithoutTexture.end());
		// No texture, warn and apply default Texture
		Log::PrintSeverity(Log::Severity::WARNING, "Applying default texture: " + warningMessageTextureType +
			" on mesh with path: \'%s\'\n", tempString.c_str());
		return m_LoadedTextures[defaultPath].second;
#endif
	}

	return nullptr;
}

SkeletonNode* AssetLoader::processSkeleton(std::map<std::string, BoneInfo> boneCounter, aiNode* assimpNode, const aiScene* assimpScene, std::map<unsigned int, VertexWeight>* perVertexBoneData)
{
	SkeletonNode* currentNode = new SkeletonNode();
	currentNode->name = assimpNode->mName.C_Str();

	// Store the default transform
	currentNode->defaultTransform = aiMatrix4x4ToXMFloat4x4(&assimpNode->mTransformation);

	// Process all bones in every mesh
	for (unsigned int i = 0; i < assimpNode->mNumMeshes; i++)
	{
		aiMesh* assimpMesh = assimpScene->mMeshes[assimpNode->mMeshes[i]];	// The aiNode only contains indices to the meshes
		processBones(boneCounter, assimpMesh, perVertexBoneData);
	}

	// Process all children and push_back the pointers
	for (unsigned int i = 0; i < assimpNode->mNumChildren; i++)
	{
		currentNode->children.push_back(processSkeleton(boneCounter, assimpNode->mChildren[i], assimpScene, perVertexBoneData));
	}

	return currentNode;
}

void AssetLoader::processBones(std::map<std::string, BoneInfo> boneCounter, const aiMesh* assimpMesh, std::map<unsigned int, VertexWeight>* perVertexBoneData)
{
	// This map keeps track of how many weights and boneIDs have been added to every vertex
	// First value is the vertexID and the second value is the amount of weights and boneIDs added to that vertex
	std::map<unsigned int, unsigned int> vertexCounter;
	for (unsigned int i = 0; i < assimpMesh->mNumBones; i++)
	{
		aiBone* assimpBone = assimpMesh->mBones[i];
		std::string boneName = assimpBone->mName.C_Str();
		// Give each bone an ID. If we already gave it an ID we don't want to change it. Also store the offset matrix.
		// This information is later stored in the SkeletonNode.
		if (boneCounter.find(boneName) != boneCounter.end())
		{
			boneCounter[boneName].boneID = boneCounter.size();
			boneCounter[boneName].boneOffset = aiMatrix4x4ToXMFloat4x4(&assimpBone->mOffsetMatrix);
		}

		// Add the vertexID and weight to the map of VertexWeights
		// This stores the data that each vertex needs later on the GPU to choose their transformations.
		for (unsigned int j = 0; j < assimpBone->mNumWeights; j++)
		{
			aiVertexWeight assimpWeight = assimpBone->mWeights[j];
			assert(vertexCounter[assimpWeight.mVertexId] < MAX_BONES_PER_VERTEX);
			// Set the bone ID in the correct vertex
			perVertexBoneData->operator[](
				assimpWeight.mVertexId).boneIDs[vertexCounter[assimpWeight.mVertexId]] = boneCounter[boneName].boneID;
			// Set the weight of the vertex
			perVertexBoneData->operator[](
				assimpWeight.mVertexId).weights[vertexCounter[assimpWeight.mVertexId]++] = assimpWeight.mWeight;
		}
	}
}

void AssetLoader::initializeSkeleton(SkeletonNode* node, std::map<std::string, BoneInfo> boneCounter, Animation* animation)
{
	// Attach the bone ID and the offset matrix to its corresponding SkeletonNode
	if (boneCounter.find(node->name) != boneCounter.end())
	{
		node->boneID = boneCounter[node->name].boneID;
		node->inverseBindPose = boneCounter[node->name].boneOffset;
	}
	
	// Set the currentStateTransform pointer. This would look nicer if we didn't need the animation to do it
	if (animation->currentState.find(node->name) != animation->currentState.end())
	{
		node->currentStateTransform = &animation->currentState[node->name].transform;
	}
	
	// Loop through all nodes in the tree
	for (auto& child : node->children)
	{
		initializeSkeleton(child, boneCounter, animation);
	}
}

Font* AssetLoader::loadFont(LPCWSTR filename, int windowWidth, int windowHeight)
{
	std::wifstream fs;
	fs.open(filename);

	m_LoadedFonts[filename].second = new Font();
	std::wstring tmp = L"";
	int startpos;

	// extract font name
	fs >> tmp >> tmp; // info face = fontname
	startpos = tmp.find(L"\"") + 1;
	m_LoadedFonts[filename].second->m_Name = tmp.substr(startpos, tmp.size() - startpos - 1);

	// get font size
	fs >> tmp;
	startpos = tmp.find(L"=") + 1;
	m_LoadedFonts[filename].second->m_Size = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing
	fs >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp;

	// get padding
	fs >> tmp;
	startpos = tmp.find(L"=") + 1;
	tmp = tmp.substr(startpos, tmp.size() - startpos); 

	// get up padding
	startpos = tmp.find(L",") + 1;
	m_LoadedFonts[filename].second->m_Toppadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	// get right padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	startpos = tmp.find(L",") + 1;
	m_LoadedFonts[filename].second->m_Rightpadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	// get down padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	startpos = tmp.find(L",") + 1;
	m_LoadedFonts[filename].second->m_Bottompadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	// get left padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	m_LoadedFonts[filename].second->m_Leftpadding = std::stoi(tmp) / (float)windowWidth;

	fs >> tmp;

	// get lineheight (how much to move down for each line), and normalize (between 0.0 and 1.0 based on size of font)
	fs >> tmp >> tmp;
	startpos = tmp.find(L"=") + 1;
	m_LoadedFonts[filename].second->m_LineHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

	// get base height (height of all characters), and normalize (between 0.0 and 1.0 based on size of font)
	fs >> tmp;
	startpos = tmp.find(L"=") + 1;
	m_LoadedFonts[filename].second->m_BaseHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

	// get texture width
	fs >> tmp;
	startpos = tmp.find(L"=") + 1;
	m_LoadedFonts[filename].second->m_TextureWidth = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// get texture height
	fs >> tmp;
	startpos = tmp.find(L"=") + 1;
	m_LoadedFonts[filename].second->m_TextureHeight = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// get pages, packed, page id
	fs >> tmp >> tmp; 
	fs >> tmp >> tmp;

	// get texture filename
	std::wstring wtmp;
	fs >> wtmp;
	startpos = wtmp.find(L"\"") + 1;
	m_LoadedFonts[filename].second->m_FontImage = wtmp.substr(startpos, wtmp.size() - startpos - 1);
	m_LoadedFonts[filename].second->m_FontImage = L"../Vendor/Resources/Fonts/" + m_LoadedFonts[filename].second->m_FontImage;

	// get number of characters
	fs >> tmp >> tmp;
	startpos = tmp.find(L"=") + 1;
	m_LoadedFonts[filename].second->m_NumCharacters = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// initialize the character list
	m_LoadedFonts[filename].second->m_pCharList = new FontChar[m_LoadedFonts[filename].second->m_NumCharacters];

	for (int c = 0; c < m_LoadedFonts[filename].second->m_NumCharacters; ++c)
	{
		// get unicode id
		fs >> tmp >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].second->m_pCharList[c].id = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get x
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].second->m_pCharList[c].u
			= (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)m_LoadedFonts[filename].second->m_TextureWidth;

		// get y
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].second->m_pCharList[c].v
			= (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)m_LoadedFonts[filename].second->m_TextureHeight;

		// get width
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		m_LoadedFonts[filename].second->m_pCharList[c].width = (float)std::stoi(tmp) / (float)windowWidth;
		m_LoadedFonts[filename].second->m_pCharList[c].twidth = (float)std::stoi(tmp) / (float)m_LoadedFonts[filename].second->m_TextureWidth;

		// get height
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		m_LoadedFonts[filename].second->m_pCharList[c].height = (float)std::stoi(tmp) / (float)windowHeight;
		m_LoadedFonts[filename].second->m_pCharList[c].theight = (float)std::stoi(tmp) / (float)m_LoadedFonts[filename].second->m_TextureHeight;

		// get xoffset
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].second->m_pCharList[c].xoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

		// get yoffset
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].second->m_pCharList[c].yoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

		// get xadvance
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].second->m_pCharList[c].xadvance = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

		// get page
		// get channel
		fs >> tmp >> tmp;
	}

	// get number of kernings
	fs >> tmp >> tmp;
	startpos = tmp.find(L"=") + 1;
	m_LoadedFonts[filename].second->m_NumKernings = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// initialize the kernings list
	m_LoadedFonts[filename].second->m_pKerningsList = new FontKerning[m_LoadedFonts[filename].second->m_NumKernings];

	for (int k = 0; k < m_LoadedFonts[filename].second->m_NumKernings; ++k)
	{
		// get first character
		fs >> tmp >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].second->m_pKerningsList[k].firstid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get second character
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		m_LoadedFonts[filename].second->m_pKerningsList[k].secondid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get amount
		fs >> tmp;
		startpos = tmp.find(L"=") + 1;
		int t = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos));
		m_LoadedFonts[filename].second->m_pKerningsList[k].amount = (float)t / (float)windowWidth;
	}

	return m_LoadedFonts[filename].second;
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

		animation->durationInTicks = assimpAnimation->mDuration;
		animation->ticksPerSecond = assimpAnimation->mTicksPerSecond != 0 ?
			assimpAnimation->mTicksPerSecond : 25.0f;

		// Store the keyframes (transform data) for each nodeAnimation (bone)
		for (unsigned int j = 0; j < assimpAnimation->mNumChannels; j++)
		{
			aiNodeAnim* assimpNodeAnimation = assimpAnimation->mChannels[j];
			std::string nodeName = assimpNodeAnimation->mNodeName.C_Str();

			// Store all the keyframes (transform data) belonging to this nodeAnimation (bone)
			for (unsigned int k = 0; k < assimpNodeAnimation->mNumPositionKeys; k++)
			{
				Keyframe key;

				key.time = assimpNodeAnimation->mPositionKeys[k].mTime;

				key.transform.position = DirectX::XMFLOAT3(
					assimpNodeAnimation->mPositionKeys[k].mValue.x,
					assimpNodeAnimation->mPositionKeys[k].mValue.y,
					assimpNodeAnimation->mPositionKeys[k].mValue.z);

				key.transform.rotationQuaternion = DirectX::XMFLOAT4(
					assimpNodeAnimation->mRotationKeys[k].mValue.x,
					assimpNodeAnimation->mRotationKeys[k].mValue.y,
					assimpNodeAnimation->mRotationKeys[k].mValue.z,
					assimpNodeAnimation->mRotationKeys[k].mValue.w);

				key.transform.scaling = DirectX::XMFLOAT3(
					assimpNodeAnimation->mScalingKeys[k].mValue.x,
					assimpNodeAnimation->mScalingKeys[k].mValue.y,
					assimpNodeAnimation->mScalingKeys[k].mValue.z);

				animation->nodeAnimationKeyframes[nodeName].push_back(key);
			}
		}

		// Save the pointer both in the model and the asset loader
		animations->push_back(animation);
		m_LoadedAnimations.push_back(animation);
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
