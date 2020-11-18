#ifndef ENGINE_H
#define ENGINE_H

// Miscellaneous
#include "Misc/Window.h"
#include "Misc/Timer.h"
#include "Misc/MultiThreading/ThreadPool.h"
#include "Misc/Option.h"
#include "Misc/NavMesh.h"

// Entity Component System
#include "ECS/SceneManager.h"
#include "ECS/Entity.h"
#include "ECS/Components/ModelComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/Lights/DirectionalLightComponent.h"
#include "ECS/Components/Lights/PointLightComponent.h"
#include "ECS/Components/Lights/SpotLightComponent.h"
#include "ECS/Components/CameraComponent.h"
#include "ECS/Components/GUI2DComponent.h"
#include "ECS/Components/SkyboxComponent.h"
#include "ECS/Components/ParticleEmitterComponent.h"
#include "ECS/Components/ProgressBarComponent.h"

// Sub-engines
#include "Renderer/Renderer.h"
#include "Renderer/Transform.h"
#include "Renderer/HeightmapModel.h"
#include "Renderer/Mesh.h"
#include "Renderer/TextManager.h"
#include "Renderer/QuadManager.h"
#include "Renderer/Camera/BaseCamera.h"
#include "Physics/Physics.h"
#include "AudioEngine/AudioBuffer.h"
#include "Particles/ParticleSystem.h"

// Textures
#include "Renderer/Material.h"
#include "Renderer/Texture/TextureCubeMap.h"
#include "Renderer/Texture/Texture2DGUI.h"

// Particle
#include "Particles/ParticleEffect.h"

// Event-handling
#include "Events/EventBus.h"

// Network
#include "Network/Network.h"

#include "Misc/AssetLoader.h"

#include "Input/Input.h"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_win32.h"
#include "ImGUI/imgui_impl_dx12.h"
#include "ImGUI/ImGuiHandler.h"

class Engine
{
public:
	Engine();
	~Engine();

	void Init(HINSTANCE hInstance, int nCmdShow);

	Window* const GetWindow() const;
	Timer* const GetTimer() const;
	ThreadPool* const GetThreadPool() const;

	Renderer* const GetRenderer() const;
	SceneManager* const GetSceneHandler() const;
	Physics* const GetPhysics() const;
	AudioEngine* const GetAudioEngine() const;
	ParticleSystem* const GetParticleSystem() const;

private:
	friend class ModelComponent;

	Window* m_pWindow = nullptr;
	Timer* m_pTimer = nullptr;
	ThreadPool* m_pThreadPool = nullptr;

	Renderer* m_pRenderer = nullptr;
	SceneManager* m_pSceneManager = nullptr;
	Physics* m_pPhysics = nullptr;
	AudioEngine* m_pAudioEngine = nullptr;
	ParticleSystem* m_pParticleSystem = nullptr;

	int m_ThreadLimit = 4;
};

#endif
